#!/usr/bin/env ruby

require 'rubygems'
require 'ffi'

module Atlast
	extend FFI::Library

	ffi_lib './libatlast.so'

	[[:atl_init, [], :void],
	 [:atl_break, [], :void],
	 [:atl_eval, [:string], :int],
	 [:atl_memstat, [], :void],
	 # These don't quite work well with FFI (or with 0.2 at least; maybe fixed):
	 # [:atl_load, [:pointer], :int],
	 # [:atl_mark, [:pointer], :void],
	 # [:atl_unwind, [:pointer], :void],
	].each { |fdef|
		attach_function *fdef
	}
end

if __FILE__ == $0
	include Atlast
	atl_init
	loop {
		print "-> "
		$stdout.flush
		atl_eval(gets || exit)
	}
end
