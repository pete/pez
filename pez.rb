#!/usr/bin/env ruby

require 'rubygems'
require 'ffi'

module Atlast
	extend FFI::Library

	ffi_lib './libpez.so'

	[[:pez_init, [], :void],
	 [:pez_break, [], :void],
	 [:pez_eval, [:string], :int],
	 [:pez_memstat, [], :void],
	 # These don't quite work well with FFI (or with 0.2 at least; maybe fixed):
	 # [:pez_load, [:pointer], :int],
	 # [:pez_mark, [:pointer], :void],
	 # [:pez_unwind, [:pointer], :void],
	].each { |fdef|
		attach_function *fdef
	}
end

if __FILE__ == $0
	include Atlast
	pez_init
	loop {
		print "-> " if $stdin.tty?
		$stdout.flush
		pez_eval(gets || exit)
	}
end
