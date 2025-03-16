REM Copyright (c) Meta Platforms, Inc. and affiliates.
REM All rights reserved.
REM
REM Licensed under the Oculus SDK License Agreement (the "License");
REM you may not use the Oculus SDK except in compliance with the License,
REM which is provided at the time of installation or download, or which
REM otherwise accompanies this software in either electronic or hard copy form.
REM
REM You may obtain a copy of the License at
REM https://developer.oculus.com/licenses/oculussdk/
REM
REM Unless required by applicable law or agreed to in writing, the Oculus SDK
REM distributed under the License is distributed on an "AS IS" BASIS,
REM WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
REM See the License for the specific language governing permissions and
REM limitations under the License.
@rem Only edit the master copy of this file in SDK_ROOT/bin/scripts/build/perproject

@setlocal enableextensions enabledelayedexpansion

@if not exist "build.gradle" (
    @echo Build script must be executed from project directory. & goto :Abort
)

@set P=..

:TryAgain

@rem @echo P = %P%

@if exist "%P%\bin\scripts\build\build.py.bat" goto :Found

@if exist "%P%\bin\scripts\build" @echo "Could not find build.py.bat" & goto :Abort

@set P=%P%\..

@goto :TryAgain

:Found

@set P=%P%\bin\scripts\build
@call %P%\build.py.bat %1 %2 %3 %4 %5
@goto :End

:Abort

:End
