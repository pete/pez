#!/usr/bin/env ruby

require 'rubygems'
require 'ffi'

module Atlast
	extend FFI::Library

	ffi_lib './libatlast.so'

	[[:atl_init, [], :void],
	 # [:atl_mark, [], :void],
	 # [:atl_unwind, [], :void],
	 [:atl_break, [], :void],
	 [:atl_eval, [:string], :int],
	 # [:atl_load, [:FILE], :int],
	 [:atl_memstat, [], :void],
	].each { |fdef|
		attach_function *fdef
	}
end
