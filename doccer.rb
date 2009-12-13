horregex = Regexp.new '\/\*\s*\((.*)\)\s+(.*)\s+\*\/\s+prim\s+(P_.*)\('

x = <<TEXT

/*
   ( addr -- *addr )
   Fetch value from address
*/
prim P_at(pez_instance *p)
{
	Sl(1);
	Hpc(S0);
	S0 = *((pez_stackitem *)S0);
}

/*
   ( n addr -- )
   Add top of stack to value at specified address
*/
prim P_plusbang(pez_instance *p)
{
	Sl(2);
	Hpc(S0);
	*((pez_stackitem *)S0) += S1;
	Pop2;
}

/*
   ( addr -- )
   Increments (by 1) the variable at the specified address.
*/
prim P_1plusbang(pez_instance *p)
{
	Sl(1);
	Hpc(S0);
	(*((pez_stackitem *)S0))++;
	Pop;
}
TEXT

x = File.read("pez.c")
require 'pp'
fns = x.scan(horregex).map { |fn| name = fn.delete_at(2); fn.unshift name  }
fns.sort! {|a,b| a.first <=> b.first }
pp *fns