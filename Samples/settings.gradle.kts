// Copyright (c) Facebook Technologies, LLC and its affiliates. All rights reserved.
rootProject.name = ("MetaOpenXRSDK")

val folderPath = "XrSamples/"

File(folderPath).listFiles()?.forEach { file ->
  if (file.isDirectory) {
    val dir = File("XrSamples/${file.name}/Projects/Android")
    include(":${file.name}")
    project(":${file.name}").projectDir = dir
  }
}
