#!/usr/bin/python3
# Copyright (c) Meta Platforms, Inc. and affiliates.
# All rights reserved.
#
# Licensed under the Oculus SDK License Agreement (the "License");
# you may not use the Oculus SDK except in compliance with the License,
# which is provided at the time of installation or download, or which
# otherwise accompanies this software in either electronic or hard copy form.
#
# You may obtain a copy of the License at
# https://developer.oculus.com/licenses/oculussdk/
#
# Unless required by applicable law or agreed to in writing, the Oculus SDK
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# This first bit of code is common bootstrapping code
# to determine the SDK root, and to set up the import
# path for additional python code.

# begin bootstrap
import os
import sys


def init():
    root = os.path.realpath(os.path.dirname(os.path.realpath(__file__)))
    os.chdir(root)  # make sure we are always executing from the project directory
    while os.path.isdir(os.path.join(root, "bin/scripts/build")) == False:
        root = os.path.realpath(os.path.join(root, ".."))
        if (
            len(root) <= 5
        ):  # Should catch both Posix and Windows root directories (e.g. '/' and 'C:\')
            print("Unable to find SDK root. Exiting.")
            sys.exit(1)
    root = os.path.abspath(root)
    os.environ["OCULUS_SDK_PATH"] = root
    sys.path.append(root + "/bin/scripts/build")


init()
import ovrbuild

ovrbuild.init()
# end bootstrap


ovrbuild.build()
