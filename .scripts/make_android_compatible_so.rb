# How to use:
# 1. ./Make.sh -lso  # to generate the lib's
# 2. ruby .scripts/make_android_compatible_so.rb

require 'fileutils'

prefix = "build/android/lib"

['armeabi-v7a', 'arm64-v8a', 'x86', 'x86_64'].each do |arch|
  FileUtils.mkdir_p("#{prefix}/#{arch}")
end

FileUtils.cp("build/armeabi-v7a/kilate", "#{prefix}/armeabi-v7a/libkilate.so")
FileUtils.cp("build/arm64-v8a/kilate",   "#{prefix}/arm64-v8a/libkilate.so")
FileUtils.cp("build/x86/kilate",         "#{prefix}/x86/libkilate.so")
FileUtils.cp("build/x86_64/kilate",      "#{prefix}/x86_64/libkilate.so")