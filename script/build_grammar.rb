#!/usr/bin/env ruby

require "plist"
require "yaml"
require "Set"
require "json"


class Environment
  def initialize()
    raise "Should only accept one parameter" unless ARGV.length == 1

    @root_dir = ARGV[0] 
	@grammar_file = File.join(@root_dir, "script/grammar.yml")
	raise "can't find grammar list file under script, checkout the project again!" unless File.exist?(@grammar_file)

	@vendor_dir = File.join(@root_dir, "vendor/grammars")
    @out_dir = File.join(@root_dir, "grammar")
  end

  def root_dir()
  	return @root_dir
  end

  def grammar_file()
  	return @grammar_file
  end

  def vendor_dir()
    return @vendor_dir
  end

  def out_dir()
    return @out_dir
  end
end

class GrammarList
  def initialize(env)
    @env = env
  end
 
  def process()
    grammar_list = YAML.load_file(@env.grammar_file)
    to_process_file_list = resolve_filelist(grammar_list)

    translate_files(to_process_file_list)
  end

  def translate_files(files)
    ensure_outputdir_exist()

    files.each do |file|
      grammar = Plist.parse_xml(file)
      raise "the grammar(#{file}) contains no scopeName, check if it's valid" unless grammar.include?("scopeName")
      write_file(grammar)
    end
  end

  def write_file(grammar)
    filename = grammar["scopeName"] + ".json"
    file = File.join(@env.out_dir, filename)

    File.open(file, "w") do |io|
      io.puts JSON.pretty_generate grammar
    end

  end

  def resolve_filelist(grammars) 
    files = Set.new
    grammars.each do |grammar|
      files.add(File.join(@env.vendor_dir, grammar["definition"]))
      files.add(File.join(@env.vendor_dir, grammar["dependency"]))
    end
    files.to_a()
  end

  def ensure_outputdir_exist()
    Dir.mkdir(@env.out_dir) unless File.exist?(@env.out_dir)
  end
end


env = Environment.new
grammars = GrammarList.new(env)
grammars.process
