
#!/bin/python3
# -*- coding: utf-8 -*-

# ##### BEGIN LICENSE BLOCK #####
# Version: MPL 1.1/GPL 2.0/LGPL 2.1
#
# The contents of this file are subject to the Mozilla Public License Version
# 1.1 (the "License"); you may not use this file except in compliance with
# the License. You may obtain a copy of the License at
# http://www.mozilla.org/MPL/
#
# Software distributed under the License is distributed on an "AS IS" basis,
# WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
# for the specific language governing rights and limitations under the
# License.
#
# The Original Code is Mozilla Universal charset detector code.
#
# The Initial Developer of the Original Code is
# Netscape Communications Corporation.
# Portions created by the Initial Developer are Copyright (C) 2001
# the Initial Developer. All Rights Reserved.
#
# Contributor(s):
#          Jehan <jehan@girinstud.io>
#
# Alternatively, the contents of this file may be used under the terms of
# either the GNU General Public License Version 2 or later (the "GPL"), or
# the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
# in which case the provisions of the GPL or the LGPL are applicable instead
# of those above. If you wish to allow use of your version of this file only
# under the terms of either the GPL or the LGPL, and not to allow others to
# use your version of this file under the terms of the MPL, indicate your
# decision by deleting the provisions above and replace them with the notice
# and other provisions required by the GPL or the LGPL. If you do not delete
# the provisions above, a recipient may use your version of this file under
# the terms of any one of the MPL, the GPL or the LGPL.
#
# ##### END LICENSE BLOCK #####

# Third party modules.
import unicodedata
import subprocess
import wikipedia
import importlib
import optparse
import datetime
import operator
import requests
import sys
import re
import os

# Custom modules.
import charsets.db
from charsets.codepoints import *

# Command line processing.
usage = 'Usage: {} <LANG-CODE>\n' \
        '\nEx: `{} fr`'.format(__file__, __file__)

description = "Internal tool for uchardet to generate language data."
cmdline = optparse.OptionParser(usage, description = description)
cmdline.add_option('--max-page',
                   help = 'Maximum number of Wikipedia pages to parse (useful for debugging).',
                   action = 'store', type = 'int', dest = 'max_page', default = None)
cmdline.add_option('--max-depth',
                   help = 'Maximum depth when following links from start page (default: 2).',
                   action = 'store', type = 'int',
                   dest = 'max_depth', default = 2)
(options, langs) = cmdline.parse_args()
if len(langs) < 1:
    print("Please select at least one language code.\n")
    exit(1)
if len(langs) > 1:
    print("This script is meant to generate data for one language at a time.\n")
    exit(1)
lang = langs[0]

# Load the language data.
sys_path_backup = sys.path
current_dir = os.path.dirname(os.path.realpath(__file__))
sys.path = [current_dir + '/langs']

try:
    lang = importlib.import_module(lang.lower())
except ImportError:
    print('Unknown language code "{}": '
          'file "langs/{}.py" does not exist.'.format(lang, lang.lower()))
    exit(1)
sys.path = sys_path_backup

charsets = charsets.db.load(lang.charsets)

if not hasattr(lang, 'start_pages') or lang.start_pages is None or \
   lang.start_pages == []:
    # Let's start with the main page, assuming it should have links
    # to relevant pages. In locale wikipedia, this page is usually redirected
    # to a relevant page.
    print("Warning: no `start_pages` set for '{}'. Using ['Main_Page'].\n"
          "         If you don't get good data, it is advised to set a "
          "start_pages` variable yourself.".format(lang.code))
    lang.start_pages = ['Main_Page']
if not hasattr(lang, 'wikipedia_code') or lang.wikipedia_code is None:
    lang.wikipedia_code = lang.code
if not hasattr(lang, 'clean_wikipedia_content') or lang.clean_wikipedia_content is None:
    lang.clean_wikipedia_content = None
if hasattr(lang, 'case_mapping'):
    lang.case_mapping = bool(lang.case_mapping)
else:
    lang.case_mapping = False
if not hasattr(lang, 'custom_case_mapping'):
    lang.custom_case_mapping = None
if not hasattr(lang, 'alphabet') or lang.alphabet is None:
    lang.alphabet = None

def local_lowercase(text, lang):
    lowercased = ''
    for l in text:
        if lang.custom_case_mapping is not None and \
           l in lang.custom_case_mapping:
            lowercased += lang.custom_case_mapping[l]
        elif l.isupper() and \
             lang.case_mapping and \
             len(unicodedata.normalize('NFC', l.lower())) == 1:
            lowercased += l.lower()