#
# Be sure to run `pod lib lint SharpHighlighter.podspec' to ensure this is a
# valid spec and remove all comments before submitting the spec.
#
# Any lines starting with a # are optional, but encouraged
#
# To learn more about a Podspec see http://guides.cocoapods.org/syntax/podspec.html
#

Pod::Spec.new do |s|
  s.name             = "SharpHighlighter"
  s.version          = "0.1.0"
  s.summary          = "A short description of SharpHighlighter."
  s.description      = <<-DESC
                       An optional longer description of SharpHighlighter

                       * Markdown format.
                       * Don't worry about the indent, we strip it!
                       DESC
  s.homepage         = "https://github.com/<GITHUB_USERNAME>/SharpHighlighter"
  # s.screenshots     = "www.example.com/screenshots_1", "www.example.com/screenshots_2"
  s.license          = 'MIT'
  s.author           = { "David" => "DevilDavidWang@gmail.com" }
  s.source           = { :git => "https://github.com/<GITHUB_USERNAME>/SharpHighlighter.git", :tag => s.version.to_s }
  # s.social_media_url = 'https://twitter.com/<TWITTER_USERNAME>'

  #s.platform     = :ios, '7.0'
  s.requires_arc = true

  # s.public_header_files = 'Pod/Classes/**/*.h'
  # s.frameworks = 'UIKit', 'MapKit'
  # s.dependency 'AFNetworking', '~> 2.3'

  s.subspec 'util' do |util|
	util.source_files = "src/util/**/*.{h,m}"
  end

  s.subspec 'core' do |util|
	util.source_files = "src/core/**/*.{h,m}"
  end

  s.subspec 'Onigmo' do |oni|
  	oni.public_header_files = "vendor/Onigmo/include"
	oni.vendored_libraries = "vendor/Onigmo/lib/libonig.a"
  end
end
