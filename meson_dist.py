#!/usr/bin/python3
# vim:set fileencoding=utf-8 et sts=4 sw=4:
#
# kasumi-unicode - a management tool for a private dictionary of anthy
#
# Copyright © 2026 Takao Fujiwara <takao.fujiwara1@gmail.com>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
# MA  02110-1301, USA.


import json
import os
import shutil
import subprocess
import sys

# Set the previous kasumi-unicode version from `git tag -l` command.
KASUMI_PREV_VERSION = os.getenv('KASUMI_PREV_VERSION')
BUILD_ROOT = os.getenv('MESON_PROJECT_BUILD_ROOT', '.')
SOURCE_ROOT = os.getenv('MESON_PROJECT_SOURCE_ROOT', '.')
DIST_ROOT = os.getenv('MESON_PROJECT_DIST_ROOT', '.')

cmd = ['meson', 'introspect', '--projectinfo', BUILD_ROOT]
info = None
try:
    result = subprocess.run(cmd, text=True, check=True,
                            stdout=subprocess.PIPE,
                            stderr=subprocess.STDOUT,
                            cwd=BUILD_ROOT).stdout
    info = json.loads(result)
except subprocess.CalledProcessError as e:
    print(f'Failed to run meson introspect: {e}', file=sys.stderr)
    sys.exit(1)
except json.JSONDecodeError as e:
    print(f'Failed to decode JSON from meson introspect: {e}',
          file=sys.stderr)
    sys.exit(1)

os.chdir(SOURCE_ROOT)
if not os.path.isdir('.git'):
    print(f'{SOURCE_ROOT}/.git is not a directory', file=sys.stderr)
    sys.exit(0)

name_str = info.get('descriptive_name')
version_str = info.get('version')
if name_str == None or version_str == None:
    print(f'You should run meson setup {BUILD_ROOT}', file=sys.stderr)
    sys.exit(1)

versions = version_str.split('.')
major_version = int(versions[0])
minor_version = int(versions[1])
if KASUMI_PREV_VERSION == None:
    prev_minor_version = minor_version - 1
    KASUMI_PREV_VERSION = '%d.%d' % (major_version, prev_minor_version)
subprocess.run('git log --name-status --date=iso > "{0}/{1}"'.format(
               DIST_ROOT, 'ChangeLog'),
               shell = True)
subprocess.run('echo "Changes in {0} {1}" > "{2}/{3}"'.format(
               name_str, version_str,
               DIST_ROOT, 'NEWS'),
               shell = True)
subprocess.run('echo "" >> "{0}/{1}"'.format(
               DIST_ROOT, 'NEWS'),
               shell = True)
subprocess.run('git shortlog {0}...{1} >> "{2}/{3}"'.format(
               KASUMI_PREV_VERSION, version_str,
               DIST_ROOT, 'NEWS'),
               shell = True)
subprocess.run('echo "" >> "{0}/{1}"'.format(
               DIST_ROOT, 'NEWS'),
               shell = True)
subprocess.run('git log {0}...{1} {2} >> "{3}/{4}"'.format(
               KASUMI_PREV_VERSION, version_str,
               '--reverse --pretty=format:"%s (%an) %h"',
               DIST_ROOT, 'NEWS'),
               shell = True)

shutil.copy2('autogen.sh', f'{DIST_ROOT}')
# Do you wish to copy configure and Makefile.in ?
