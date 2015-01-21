Pod::Spec.new do |s|
  s.name         = "SharpHighlighter"
  s.version      = "0.0.1"
  s.summary      = "A syntax highlighter for iOS and OSX."
  s.description  = <<-DESC
				   SharpHighliter is syntax highlighter that highlights many languages.
				   SharpHighliter also supports user to define customized syntax/theme.
                   DESC
  s.homepage     = "https://github.com/dahakawang/SharpHighlighter"
  s.license      = "MIT"
  s.author             = { "David" => "DevilDavidWang@gmail.com" }
  s.ios.deployment_target = "5.0"
  s.osx.deployment_target = "10.7"
  s.source       = { :git => "https://github.com/dahakawang/SharpHighlighter.git", :tag => "0.0.1" }

  #s.source_files  = "SharpHighlighter/**/*.{h,m}"

  s.subspec 'Core' do |ss|
  	ss.source_files = "SharpHighlighter/core/**/*.{h,m}"
  end

  s.subspec 'Exception' do |ss|
  	ss.source_files = "SharpHighlighter/exception/**/*.{h,m}"
  end

  s.subspec 'Renderer' do |ss|
  	ss.source_files = "SharpHighlighter/renderer/**/*.{h,m}"
  end

  s.subspec 'Theme' do |ss|
  	ss.source_files = "SharpHighlighter/theme/**/*.{h,m}"
  end
  # s.public_header_files = "Classes/**/*.h"

  # ――― Resources ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― #
  #
  #  A list of resources included with the Pod. These are copied into the
  #  target bundle with a build phase script. Anything else will be cleaned.
  #  You can preserve files from being cleaned, please don't preserve
  #  non-essential files like tests, examples and documentation.
  #

  # s.resource  = "icon.png"
  # s.resources = "Resources/*.png"

  # s.preserve_paths = "FilesToSave", "MoreFilesToSave"


  # ――― Project Linking ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――― #
  #
  #  Link your library with frameworks, or libraries. Libraries do not include
  #  the lib prefix of their name.
  #

  # s.framework  = "SomeFramework"
  # s.frameworks = "SomeFramework", "AnotherFramework"

  # s.library   = "iconv"
  # s.libraries = "iconv", "xml2"


  # ――― Project Settings ――――――――――――――――――――――――――――――――――――――――――――――――――――――――― #
  #
  #  If your library depends on compiler flags you can set them in the xcconfig hash
  #  where they will only apply to your library. If you depend on other Podspecs
  #  you can include multiple dependencies to ensure it works.

  # s.requires_arc = true

  # s.xcconfig = { "HEADER_SEARCH_PATHS" => "$(SDKROOT)/usr/include/libxml2" }
  # s.dependency "JSONKit", "~> 1.4"

end
