/* type_primitives.c
   This file has been automatically generated as part of the Pez build
   process, by typegen.pez.  It defines a number of primitive functions
   that help out with dealing with the variously sized types presented by
   C, and including them in structs.

   So editing this file is going to be unhelpful; you'd be better off
   editing typegen.pez.  So that Pez can be built on machines that don't
   already have Pez, this file is included in the repository and with
   distribution tarballs.
*/

prim P_c_int_at(pez_instance *p)
{
	Sl(1);
	Hpc(S0);
	S0 = *(int *)S0;
}

prim P_c_int_bang(pez_instance *p)
{
	Sl(2);
	Hpc(S0);
	*(int *)S0 = (int)S1;
	Npop(2);
}

prim P_c_int_size(pez_instance *p)
{
	So(1);
	Push = sizeof(int);
}

prim P_c_ints(pez_instance *p)
{
	Sl(1);
	S0 *= sizeof(int);
}

prim P_c_int_colon(pez_instance *p)
{
	pez_dictword *offsetw = (pez_dictword *)p->hptr, *w;
	pez_stackitem plus;
	int namelen, token;
	char *name, *buf;
	
	if(!p->createstruct) {
		trouble(p, "Tried to define struct member outside struct "
			"definition");
		return;
	}
	plus = (pez_stackitem)lookup(p, "+");
	
	name = alloc(TOK_BUF_SZ);
	token = lex(p, &p->instream, name);
	if(token != TokWord) {
		trouble(p, "Expected a word to follow 'c-int:'");
		return;
	}
	namelen = strlen(name);
	
	// ( struct-addr -- member-addr ) The offset-adder:
	Ho(Dictwordl);
	p->hptr += Dictwordl;
	buf = alloc(namelen + 2);
	memcpy(buf + 1, name, namelen);
	offsetw->wname = buf;
	offsetw->wcode = P_nest;
	offsetw->wnext = p->dict;
	p->dict = offsetw;
	if(p->createstruct->size) { // No point in adding zero.
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle(s_exit);
	
	// ( struct-addr -- member ) The reader:
	Ho(Dictwordl);
	buf = alloc(namelen + 3);
	sprintf(buf + 1, "%s@", name);
	w = (pez_dictword *)p->hptr;
	p->hptr += Dictwordl;
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) {
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle((pez_stackitem)lookup(p, "c-int@"));
	Hsingle(s_exit);
	
	// ( val struct-addr -- ) The writer:
	Ho(Dictwordl);
	w = (pez_dictword *)p->hptr;
	p->hptr += Dictwordl;
	buf = alloc(namelen + 3);
	sprintf(buf + 1, "%s!", name);
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) {
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle((pez_stackitem)lookup(p, "c-int!"));
	Hsingle(s_exit);
	
	// And, finally, we add the size of a c-int to the size of the struct.
	p->createstruct->size += sizeof(int);
}

prim P_c_ints_colon(pez_instance *p)
{
	pez_dictword *w = (pez_dictword *)p->hptr;
	pez_stackitem plus;
	int namelen, token;
	char *name, *buf;
	
	if(!p->createstruct) {
		trouble(p, "Tried to define struct member outside struct "
			"definition");
		return;
	}
	Sl(1);
	
	plus = (pez_stackitem)lookup(p, "+");
	
	name = alloc(TOK_BUF_SZ);
	token = lex(p, &p->instream, name);
	if(token != TokWord) {
		trouble(p, "Expected a word to follow 'c-ints:'");
		return;
	}
	namelen = strlen(name);
	
	Ho(Dictwordl);
	p->hptr += Dictwordl;
	buf = alloc(namelen + 2);
	memcpy(buf + 1, name, namelen);
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) { // No point in adding zero.
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle(s_exit);
	
	p->createstruct->size += S0 * sizeof(int);
	Pop;
}

prim P_c_long_at(pez_instance *p)
{
	Sl(1);
	Hpc(S0);
	S0 = *(long *)S0;
}

prim P_c_long_bang(pez_instance *p)
{
	Sl(2);
	Hpc(S0);
	*(long *)S0 = (long)S1;
	Npop(2);
}

prim P_c_long_size(pez_instance *p)
{
	So(1);
	Push = sizeof(long);
}

prim P_c_longs(pez_instance *p)
{
	Sl(1);
	S0 *= sizeof(long);
}

prim P_c_long_colon(pez_instance *p)
{
	pez_dictword *offsetw = (pez_dictword *)p->hptr, *w;
	pez_stackitem plus;
	int namelen, token;
	char *name, *buf;
	
	if(!p->createstruct) {
		trouble(p, "Tried to define struct member outside struct "
			"definition");
		return;
	}
	plus = (pez_stackitem)lookup(p, "+");
	
	name = alloc(TOK_BUF_SZ);
	token = lex(p, &p->instream, name);
	if(token != TokWord) {
		trouble(p, "Expected a word to follow 'c-long:'");
		return;
	}
	namelen = strlen(name);
	
	// ( struct-addr -- member-addr ) The offset-adder:
	Ho(Dictwordl);
	p->hptr += Dictwordl;
	buf = alloc(namelen + 2);
	memcpy(buf + 1, name, namelen);
	offsetw->wname = buf;
	offsetw->wcode = P_nest;
	offsetw->wnext = p->dict;
	p->dict = offsetw;
	if(p->createstruct->size) { // No point in adding zero.
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle(s_exit);
	
	// ( struct-addr -- member ) The reader:
	Ho(Dictwordl);
	buf = alloc(namelen + 3);
	sprintf(buf + 1, "%s@", name);
	w = (pez_dictword *)p->hptr;
	p->hptr += Dictwordl;
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) {
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle((pez_stackitem)lookup(p, "c-long@"));
	Hsingle(s_exit);
	
	// ( val struct-addr -- ) The writer:
	Ho(Dictwordl);
	w = (pez_dictword *)p->hptr;
	p->hptr += Dictwordl;
	buf = alloc(namelen + 3);
	sprintf(buf + 1, "%s!", name);
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) {
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle((pez_stackitem)lookup(p, "c-long!"));
	Hsingle(s_exit);
	
	// And, finally, we add the size of a c-long to the size of the struct.
	p->createstruct->size += sizeof(long);
}

prim P_c_longs_colon(pez_instance *p)
{
	pez_dictword *w = (pez_dictword *)p->hptr;
	pez_stackitem plus;
	int namelen, token;
	char *name, *buf;
	
	if(!p->createstruct) {
		trouble(p, "Tried to define struct member outside struct "
			"definition");
		return;
	}
	Sl(1);
	
	plus = (pez_stackitem)lookup(p, "+");
	
	name = alloc(TOK_BUF_SZ);
	token = lex(p, &p->instream, name);
	if(token != TokWord) {
		trouble(p, "Expected a word to follow 'c-longs:'");
		return;
	}
	namelen = strlen(name);
	
	Ho(Dictwordl);
	p->hptr += Dictwordl;
	buf = alloc(namelen + 2);
	memcpy(buf + 1, name, namelen);
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) { // No point in adding zero.
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle(s_exit);
	
	p->createstruct->size += S0 * sizeof(long);
	Pop;
}

prim P_c_short_at(pez_instance *p)
{
	Sl(1);
	Hpc(S0);
	S0 = *(short *)S0;
}

prim P_c_short_bang(pez_instance *p)
{
	Sl(2);
	Hpc(S0);
	*(short *)S0 = (short)S1;
	Npop(2);
}

prim P_c_short_size(pez_instance *p)
{
	So(1);
	Push = sizeof(short);
}

prim P_c_shorts(pez_instance *p)
{
	Sl(1);
	S0 *= sizeof(short);
}

prim P_c_short_colon(pez_instance *p)
{
	pez_dictword *offsetw = (pez_dictword *)p->hptr, *w;
	pez_stackitem plus;
	int namelen, token;
	char *name, *buf;
	
	if(!p->createstruct) {
		trouble(p, "Tried to define struct member outside struct "
			"definition");
		return;
	}
	plus = (pez_stackitem)lookup(p, "+");
	
	name = alloc(TOK_BUF_SZ);
	token = lex(p, &p->instream, name);
	if(token != TokWord) {
		trouble(p, "Expected a word to follow 'c-short:'");
		return;
	}
	namelen = strlen(name);
	
	// ( struct-addr -- member-addr ) The offset-adder:
	Ho(Dictwordl);
	p->hptr += Dictwordl;
	buf = alloc(namelen + 2);
	memcpy(buf + 1, name, namelen);
	offsetw->wname = buf;
	offsetw->wcode = P_nest;
	offsetw->wnext = p->dict;
	p->dict = offsetw;
	if(p->createstruct->size) { // No point in adding zero.
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle(s_exit);
	
	// ( struct-addr -- member ) The reader:
	Ho(Dictwordl);
	buf = alloc(namelen + 3);
	sprintf(buf + 1, "%s@", name);
	w = (pez_dictword *)p->hptr;
	p->hptr += Dictwordl;
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) {
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle((pez_stackitem)lookup(p, "c-short@"));
	Hsingle(s_exit);
	
	// ( val struct-addr -- ) The writer:
	Ho(Dictwordl);
	w = (pez_dictword *)p->hptr;
	p->hptr += Dictwordl;
	buf = alloc(namelen + 3);
	sprintf(buf + 1, "%s!", name);
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) {
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle((pez_stackitem)lookup(p, "c-short!"));
	Hsingle(s_exit);
	
	// And, finally, we add the size of a c-short to the size of the struct.
	p->createstruct->size += sizeof(short);
}

prim P_c_shorts_colon(pez_instance *p)
{
	pez_dictword *w = (pez_dictword *)p->hptr;
	pez_stackitem plus;
	int namelen, token;
	char *name, *buf;
	
	if(!p->createstruct) {
		trouble(p, "Tried to define struct member outside struct "
			"definition");
		return;
	}
	Sl(1);
	
	plus = (pez_stackitem)lookup(p, "+");
	
	name = alloc(TOK_BUF_SZ);
	token = lex(p, &p->instream, name);
	if(token != TokWord) {
		trouble(p, "Expected a word to follow 'c-shorts:'");
		return;
	}
	namelen = strlen(name);
	
	Ho(Dictwordl);
	p->hptr += Dictwordl;
	buf = alloc(namelen + 2);
	memcpy(buf + 1, name, namelen);
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) { // No point in adding zero.
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle(s_exit);
	
	p->createstruct->size += S0 * sizeof(short);
	Pop;
}

prim P_c_uint_at(pez_instance *p)
{
	Sl(1);
	Hpc(S0);
	S0 = *(unsigned int *)S0;
}

prim P_c_uint_bang(pez_instance *p)
{
	Sl(2);
	Hpc(S0);
	*(unsigned int *)S0 = (unsigned int)S1;
	Npop(2);
}

prim P_c_uint_size(pez_instance *p)
{
	So(1);
	Push = sizeof(unsigned int);
}

prim P_c_uints(pez_instance *p)
{
	Sl(1);
	S0 *= sizeof(unsigned int);
}

prim P_c_uint_colon(pez_instance *p)
{
	pez_dictword *offsetw = (pez_dictword *)p->hptr, *w;
	pez_stackitem plus;
	int namelen, token;
	char *name, *buf;
	
	if(!p->createstruct) {
		trouble(p, "Tried to define struct member outside struct "
			"definition");
		return;
	}
	plus = (pez_stackitem)lookup(p, "+");
	
	name = alloc(TOK_BUF_SZ);
	token = lex(p, &p->instream, name);
	if(token != TokWord) {
		trouble(p, "Expected a word to follow 'c-uint:'");
		return;
	}
	namelen = strlen(name);
	
	// ( struct-addr -- member-addr ) The offset-adder:
	Ho(Dictwordl);
	p->hptr += Dictwordl;
	buf = alloc(namelen + 2);
	memcpy(buf + 1, name, namelen);
	offsetw->wname = buf;
	offsetw->wcode = P_nest;
	offsetw->wnext = p->dict;
	p->dict = offsetw;
	if(p->createstruct->size) { // No point in adding zero.
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle(s_exit);
	
	// ( struct-addr -- member ) The reader:
	Ho(Dictwordl);
	buf = alloc(namelen + 3);
	sprintf(buf + 1, "%s@", name);
	w = (pez_dictword *)p->hptr;
	p->hptr += Dictwordl;
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) {
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle((pez_stackitem)lookup(p, "c-uint@"));
	Hsingle(s_exit);
	
	// ( val struct-addr -- ) The writer:
	Ho(Dictwordl);
	w = (pez_dictword *)p->hptr;
	p->hptr += Dictwordl;
	buf = alloc(namelen + 3);
	sprintf(buf + 1, "%s!", name);
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) {
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle((pez_stackitem)lookup(p, "c-uint!"));
	Hsingle(s_exit);
	
	// And, finally, we add the size of a c-uint to the size of the struct.
	p->createstruct->size += sizeof(unsigned int);
}

prim P_c_uints_colon(pez_instance *p)
{
	pez_dictword *w = (pez_dictword *)p->hptr;
	pez_stackitem plus;
	int namelen, token;
	char *name, *buf;
	
	if(!p->createstruct) {
		trouble(p, "Tried to define struct member outside struct "
			"definition");
		return;
	}
	Sl(1);
	
	plus = (pez_stackitem)lookup(p, "+");
	
	name = alloc(TOK_BUF_SZ);
	token = lex(p, &p->instream, name);
	if(token != TokWord) {
		trouble(p, "Expected a word to follow 'c-uints:'");
		return;
	}
	namelen = strlen(name);
	
	Ho(Dictwordl);
	p->hptr += Dictwordl;
	buf = alloc(namelen + 2);
	memcpy(buf + 1, name, namelen);
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) { // No point in adding zero.
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle(s_exit);
	
	p->createstruct->size += S0 * sizeof(unsigned int);
	Pop;
}

prim P_c_ulong_at(pez_instance *p)
{
	Sl(1);
	Hpc(S0);
	S0 = *(unsigned long *)S0;
}

prim P_c_ulong_bang(pez_instance *p)
{
	Sl(2);
	Hpc(S0);
	*(unsigned long *)S0 = (unsigned long)S1;
	Npop(2);
}

prim P_c_ulong_size(pez_instance *p)
{
	So(1);
	Push = sizeof(unsigned long);
}

prim P_c_ulongs(pez_instance *p)
{
	Sl(1);
	S0 *= sizeof(unsigned long);
}

prim P_c_ulong_colon(pez_instance *p)
{
	pez_dictword *offsetw = (pez_dictword *)p->hptr, *w;
	pez_stackitem plus;
	int namelen, token;
	char *name, *buf;
	
	if(!p->createstruct) {
		trouble(p, "Tried to define struct member outside struct "
			"definition");
		return;
	}
	plus = (pez_stackitem)lookup(p, "+");
	
	name = alloc(TOK_BUF_SZ);
	token = lex(p, &p->instream, name);
	if(token != TokWord) {
		trouble(p, "Expected a word to follow 'c-ulong:'");
		return;
	}
	namelen = strlen(name);
	
	// ( struct-addr -- member-addr ) The offset-adder:
	Ho(Dictwordl);
	p->hptr += Dictwordl;
	buf = alloc(namelen + 2);
	memcpy(buf + 1, name, namelen);
	offsetw->wname = buf;
	offsetw->wcode = P_nest;
	offsetw->wnext = p->dict;
	p->dict = offsetw;
	if(p->createstruct->size) { // No point in adding zero.
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle(s_exit);
	
	// ( struct-addr -- member ) The reader:
	Ho(Dictwordl);
	buf = alloc(namelen + 3);
	sprintf(buf + 1, "%s@", name);
	w = (pez_dictword *)p->hptr;
	p->hptr += Dictwordl;
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) {
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle((pez_stackitem)lookup(p, "c-ulong@"));
	Hsingle(s_exit);
	
	// ( val struct-addr -- ) The writer:
	Ho(Dictwordl);
	w = (pez_dictword *)p->hptr;
	p->hptr += Dictwordl;
	buf = alloc(namelen + 3);
	sprintf(buf + 1, "%s!", name);
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) {
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle((pez_stackitem)lookup(p, "c-ulong!"));
	Hsingle(s_exit);
	
	// And, finally, we add the size of a c-ulong to the size of the struct.
	p->createstruct->size += sizeof(unsigned long);
}

prim P_c_ulongs_colon(pez_instance *p)
{
	pez_dictword *w = (pez_dictword *)p->hptr;
	pez_stackitem plus;
	int namelen, token;
	char *name, *buf;
	
	if(!p->createstruct) {
		trouble(p, "Tried to define struct member outside struct "
			"definition");
		return;
	}
	Sl(1);
	
	plus = (pez_stackitem)lookup(p, "+");
	
	name = alloc(TOK_BUF_SZ);
	token = lex(p, &p->instream, name);
	if(token != TokWord) {
		trouble(p, "Expected a word to follow 'c-ulongs:'");
		return;
	}
	namelen = strlen(name);
	
	Ho(Dictwordl);
	p->hptr += Dictwordl;
	buf = alloc(namelen + 2);
	memcpy(buf + 1, name, namelen);
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) { // No point in adding zero.
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle(s_exit);
	
	p->createstruct->size += S0 * sizeof(unsigned long);
	Pop;
}

prim P_c_ushort_at(pez_instance *p)
{
	Sl(1);
	Hpc(S0);
	S0 = *(unsigned short *)S0;
}

prim P_c_ushort_bang(pez_instance *p)
{
	Sl(2);
	Hpc(S0);
	*(unsigned short *)S0 = (unsigned short)S1;
	Npop(2);
}

prim P_c_ushort_size(pez_instance *p)
{
	So(1);
	Push = sizeof(unsigned short);
}

prim P_c_ushorts(pez_instance *p)
{
	Sl(1);
	S0 *= sizeof(unsigned short);
}

prim P_c_ushort_colon(pez_instance *p)
{
	pez_dictword *offsetw = (pez_dictword *)p->hptr, *w;
	pez_stackitem plus;
	int namelen, token;
	char *name, *buf;
	
	if(!p->createstruct) {
		trouble(p, "Tried to define struct member outside struct "
			"definition");
		return;
	}
	plus = (pez_stackitem)lookup(p, "+");
	
	name = alloc(TOK_BUF_SZ);
	token = lex(p, &p->instream, name);
	if(token != TokWord) {
		trouble(p, "Expected a word to follow 'c-ushort:'");
		return;
	}
	namelen = strlen(name);
	
	// ( struct-addr -- member-addr ) The offset-adder:
	Ho(Dictwordl);
	p->hptr += Dictwordl;
	buf = alloc(namelen + 2);
	memcpy(buf + 1, name, namelen);
	offsetw->wname = buf;
	offsetw->wcode = P_nest;
	offsetw->wnext = p->dict;
	p->dict = offsetw;
	if(p->createstruct->size) { // No point in adding zero.
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle(s_exit);
	
	// ( struct-addr -- member ) The reader:
	Ho(Dictwordl);
	buf = alloc(namelen + 3);
	sprintf(buf + 1, "%s@", name);
	w = (pez_dictword *)p->hptr;
	p->hptr += Dictwordl;
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) {
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle((pez_stackitem)lookup(p, "c-ushort@"));
	Hsingle(s_exit);
	
	// ( val struct-addr -- ) The writer:
	Ho(Dictwordl);
	w = (pez_dictword *)p->hptr;
	p->hptr += Dictwordl;
	buf = alloc(namelen + 3);
	sprintf(buf + 1, "%s!", name);
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) {
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle((pez_stackitem)lookup(p, "c-ushort!"));
	Hsingle(s_exit);
	
	// And, finally, we add the size of a c-ushort to the size of the struct.
	p->createstruct->size += sizeof(unsigned short);
}

prim P_c_ushorts_colon(pez_instance *p)
{
	pez_dictword *w = (pez_dictword *)p->hptr;
	pez_stackitem plus;
	int namelen, token;
	char *name, *buf;
	
	if(!p->createstruct) {
		trouble(p, "Tried to define struct member outside struct "
			"definition");
		return;
	}
	Sl(1);
	
	plus = (pez_stackitem)lookup(p, "+");
	
	name = alloc(TOK_BUF_SZ);
	token = lex(p, &p->instream, name);
	if(token != TokWord) {
		trouble(p, "Expected a word to follow 'c-ushorts:'");
		return;
	}
	namelen = strlen(name);
	
	Ho(Dictwordl);
	p->hptr += Dictwordl;
	buf = alloc(namelen + 2);
	memcpy(buf + 1, name, namelen);
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) { // No point in adding zero.
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle(s_exit);
	
	p->createstruct->size += S0 * sizeof(unsigned short);
	Pop;
}

prim P_c_float_at(pez_instance *p)
{
	Sl(1);
	Hpc(S0);
	S0 = *(float *)S0;
}

prim P_c_float_bang(pez_instance *p)
{
	Sl(2);
	Hpc(S0);
	*(float *)S0 = (float)S1;
	Npop(2);
}

prim P_c_float_size(pez_instance *p)
{
	So(1);
	Push = sizeof(float);
}

prim P_c_floats(pez_instance *p)
{
	Sl(1);
	S0 *= sizeof(float);
}

prim P_c_float_colon(pez_instance *p)
{
	pez_dictword *offsetw = (pez_dictword *)p->hptr, *w;
	pez_stackitem plus;
	int namelen, token;
	char *name, *buf;
	
	if(!p->createstruct) {
		trouble(p, "Tried to define struct member outside struct "
			"definition");
		return;
	}
	plus = (pez_stackitem)lookup(p, "+");
	
	name = alloc(TOK_BUF_SZ);
	token = lex(p, &p->instream, name);
	if(token != TokWord) {
		trouble(p, "Expected a word to follow 'c-float:'");
		return;
	}
	namelen = strlen(name);
	
	// ( struct-addr -- member-addr ) The offset-adder:
	Ho(Dictwordl);
	p->hptr += Dictwordl;
	buf = alloc(namelen + 2);
	memcpy(buf + 1, name, namelen);
	offsetw->wname = buf;
	offsetw->wcode = P_nest;
	offsetw->wnext = p->dict;
	p->dict = offsetw;
	if(p->createstruct->size) { // No point in adding zero.
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle(s_exit);
	
	// ( struct-addr -- member ) The reader:
	Ho(Dictwordl);
	buf = alloc(namelen + 3);
	sprintf(buf + 1, "%s@", name);
	w = (pez_dictword *)p->hptr;
	p->hptr += Dictwordl;
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) {
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle((pez_stackitem)lookup(p, "c-float@"));
	Hsingle(s_exit);
	
	// ( val struct-addr -- ) The writer:
	Ho(Dictwordl);
	w = (pez_dictword *)p->hptr;
	p->hptr += Dictwordl;
	buf = alloc(namelen + 3);
	sprintf(buf + 1, "%s!", name);
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) {
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle((pez_stackitem)lookup(p, "c-float!"));
	Hsingle(s_exit);
	
	// And, finally, we add the size of a c-float to the size of the struct.
	p->createstruct->size += sizeof(float);
}

prim P_c_floats_colon(pez_instance *p)
{
	pez_dictword *w = (pez_dictword *)p->hptr;
	pez_stackitem plus;
	int namelen, token;
	char *name, *buf;
	
	if(!p->createstruct) {
		trouble(p, "Tried to define struct member outside struct "
			"definition");
		return;
	}
	Sl(1);
	
	plus = (pez_stackitem)lookup(p, "+");
	
	name = alloc(TOK_BUF_SZ);
	token = lex(p, &p->instream, name);
	if(token != TokWord) {
		trouble(p, "Expected a word to follow 'c-floats:'");
		return;
	}
	namelen = strlen(name);
	
	Ho(Dictwordl);
	p->hptr += Dictwordl;
	buf = alloc(namelen + 2);
	memcpy(buf + 1, name, namelen);
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) { // No point in adding zero.
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle(s_exit);
	
	p->createstruct->size += S0 * sizeof(float);
	Pop;
}

prim P_c_double_at(pez_instance *p)
{
	Sl(1);
	Hpc(S0);
	S0 = *(double *)S0;
}

prim P_c_double_bang(pez_instance *p)
{
	Sl(2);
	Hpc(S0);
	*(double *)S0 = (double)S1;
	Npop(2);
}

prim P_c_double_size(pez_instance *p)
{
	So(1);
	Push = sizeof(double);
}

prim P_c_doubles(pez_instance *p)
{
	Sl(1);
	S0 *= sizeof(double);
}

prim P_c_double_colon(pez_instance *p)
{
	pez_dictword *offsetw = (pez_dictword *)p->hptr, *w;
	pez_stackitem plus;
	int namelen, token;
	char *name, *buf;
	
	if(!p->createstruct) {
		trouble(p, "Tried to define struct member outside struct "
			"definition");
		return;
	}
	plus = (pez_stackitem)lookup(p, "+");
	
	name = alloc(TOK_BUF_SZ);
	token = lex(p, &p->instream, name);
	if(token != TokWord) {
		trouble(p, "Expected a word to follow 'c-double:'");
		return;
	}
	namelen = strlen(name);
	
	// ( struct-addr -- member-addr ) The offset-adder:
	Ho(Dictwordl);
	p->hptr += Dictwordl;
	buf = alloc(namelen + 2);
	memcpy(buf + 1, name, namelen);
	offsetw->wname = buf;
	offsetw->wcode = P_nest;
	offsetw->wnext = p->dict;
	p->dict = offsetw;
	if(p->createstruct->size) { // No point in adding zero.
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle(s_exit);
	
	// ( struct-addr -- member ) The reader:
	Ho(Dictwordl);
	buf = alloc(namelen + 3);
	sprintf(buf + 1, "%s@", name);
	w = (pez_dictword *)p->hptr;
	p->hptr += Dictwordl;
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) {
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle((pez_stackitem)lookup(p, "c-double@"));
	Hsingle(s_exit);
	
	// ( val struct-addr -- ) The writer:
	Ho(Dictwordl);
	w = (pez_dictword *)p->hptr;
	p->hptr += Dictwordl;
	buf = alloc(namelen + 3);
	sprintf(buf + 1, "%s!", name);
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) {
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle((pez_stackitem)lookup(p, "c-double!"));
	Hsingle(s_exit);
	
	// And, finally, we add the size of a c-double to the size of the struct.
	p->createstruct->size += sizeof(double);
}

prim P_c_doubles_colon(pez_instance *p)
{
	pez_dictword *w = (pez_dictword *)p->hptr;
	pez_stackitem plus;
	int namelen, token;
	char *name, *buf;
	
	if(!p->createstruct) {
		trouble(p, "Tried to define struct member outside struct "
			"definition");
		return;
	}
	Sl(1);
	
	plus = (pez_stackitem)lookup(p, "+");
	
	name = alloc(TOK_BUF_SZ);
	token = lex(p, &p->instream, name);
	if(token != TokWord) {
		trouble(p, "Expected a word to follow 'c-doubles:'");
		return;
	}
	namelen = strlen(name);
	
	Ho(Dictwordl);
	p->hptr += Dictwordl;
	buf = alloc(namelen + 2);
	memcpy(buf + 1, name, namelen);
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) { // No point in adding zero.
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle(s_exit);
	
	p->createstruct->size += S0 * sizeof(double);
	Pop;
}

prim P_int8_at(pez_instance *p)
{
	Sl(1);
	Hpc(S0);
	S0 = *(int8_t *)S0;
}

prim P_int8_bang(pez_instance *p)
{
	Sl(2);
	Hpc(S0);
	*(int8_t *)S0 = (int8_t)S1;
	Npop(2);
}

prim P_int8_size(pez_instance *p)
{
	So(1);
	Push = sizeof(int8_t);
}

prim P_int8s(pez_instance *p)
{
	Sl(1);
	S0 *= sizeof(int8_t);
}

prim P_int8_colon(pez_instance *p)
{
	pez_dictword *offsetw = (pez_dictword *)p->hptr, *w;
	pez_stackitem plus;
	int namelen, token;
	char *name, *buf;
	
	if(!p->createstruct) {
		trouble(p, "Tried to define struct member outside struct "
			"definition");
		return;
	}
	plus = (pez_stackitem)lookup(p, "+");
	
	name = alloc(TOK_BUF_SZ);
	token = lex(p, &p->instream, name);
	if(token != TokWord) {
		trouble(p, "Expected a word to follow 'int8:'");
		return;
	}
	namelen = strlen(name);
	
	// ( struct-addr -- member-addr ) The offset-adder:
	Ho(Dictwordl);
	p->hptr += Dictwordl;
	buf = alloc(namelen + 2);
	memcpy(buf + 1, name, namelen);
	offsetw->wname = buf;
	offsetw->wcode = P_nest;
	offsetw->wnext = p->dict;
	p->dict = offsetw;
	if(p->createstruct->size) { // No point in adding zero.
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle(s_exit);
	
	// ( struct-addr -- member ) The reader:
	Ho(Dictwordl);
	buf = alloc(namelen + 3);
	sprintf(buf + 1, "%s@", name);
	w = (pez_dictword *)p->hptr;
	p->hptr += Dictwordl;
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) {
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle((pez_stackitem)lookup(p, "int8@"));
	Hsingle(s_exit);
	
	// ( val struct-addr -- ) The writer:
	Ho(Dictwordl);
	w = (pez_dictword *)p->hptr;
	p->hptr += Dictwordl;
	buf = alloc(namelen + 3);
	sprintf(buf + 1, "%s!", name);
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) {
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle((pez_stackitem)lookup(p, "int8!"));
	Hsingle(s_exit);
	
	// And, finally, we add the size of a int8 to the size of the struct.
	p->createstruct->size += sizeof(int8_t);
}

prim P_int8s_colon(pez_instance *p)
{
	pez_dictword *w = (pez_dictword *)p->hptr;
	pez_stackitem plus;
	int namelen, token;
	char *name, *buf;
	
	if(!p->createstruct) {
		trouble(p, "Tried to define struct member outside struct "
			"definition");
		return;
	}
	Sl(1);
	
	plus = (pez_stackitem)lookup(p, "+");
	
	name = alloc(TOK_BUF_SZ);
	token = lex(p, &p->instream, name);
	if(token != TokWord) {
		trouble(p, "Expected a word to follow 'int8s:'");
		return;
	}
	namelen = strlen(name);
	
	Ho(Dictwordl);
	p->hptr += Dictwordl;
	buf = alloc(namelen + 2);
	memcpy(buf + 1, name, namelen);
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) { // No point in adding zero.
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle(s_exit);
	
	p->createstruct->size += S0 * sizeof(int8_t);
	Pop;
}

prim P_uint8_at(pez_instance *p)
{
	Sl(1);
	Hpc(S0);
	S0 = *(uint8_t *)S0;
}

prim P_uint8_bang(pez_instance *p)
{
	Sl(2);
	Hpc(S0);
	*(uint8_t *)S0 = (uint8_t)S1;
	Npop(2);
}

prim P_uint8_size(pez_instance *p)
{
	So(1);
	Push = sizeof(uint8_t);
}

prim P_uint8s(pez_instance *p)
{
	Sl(1);
	S0 *= sizeof(uint8_t);
}

prim P_uint8_colon(pez_instance *p)
{
	pez_dictword *offsetw = (pez_dictword *)p->hptr, *w;
	pez_stackitem plus;
	int namelen, token;
	char *name, *buf;
	
	if(!p->createstruct) {
		trouble(p, "Tried to define struct member outside struct "
			"definition");
		return;
	}
	plus = (pez_stackitem)lookup(p, "+");
	
	name = alloc(TOK_BUF_SZ);
	token = lex(p, &p->instream, name);
	if(token != TokWord) {
		trouble(p, "Expected a word to follow 'uint8:'");
		return;
	}
	namelen = strlen(name);
	
	// ( struct-addr -- member-addr ) The offset-adder:
	Ho(Dictwordl);
	p->hptr += Dictwordl;
	buf = alloc(namelen + 2);
	memcpy(buf + 1, name, namelen);
	offsetw->wname = buf;
	offsetw->wcode = P_nest;
	offsetw->wnext = p->dict;
	p->dict = offsetw;
	if(p->createstruct->size) { // No point in adding zero.
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle(s_exit);
	
	// ( struct-addr -- member ) The reader:
	Ho(Dictwordl);
	buf = alloc(namelen + 3);
	sprintf(buf + 1, "%s@", name);
	w = (pez_dictword *)p->hptr;
	p->hptr += Dictwordl;
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) {
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle((pez_stackitem)lookup(p, "uint8@"));
	Hsingle(s_exit);
	
	// ( val struct-addr -- ) The writer:
	Ho(Dictwordl);
	w = (pez_dictword *)p->hptr;
	p->hptr += Dictwordl;
	buf = alloc(namelen + 3);
	sprintf(buf + 1, "%s!", name);
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) {
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle((pez_stackitem)lookup(p, "uint8!"));
	Hsingle(s_exit);
	
	// And, finally, we add the size of a uint8 to the size of the struct.
	p->createstruct->size += sizeof(uint8_t);
}

prim P_uint8s_colon(pez_instance *p)
{
	pez_dictword *w = (pez_dictword *)p->hptr;
	pez_stackitem plus;
	int namelen, token;
	char *name, *buf;
	
	if(!p->createstruct) {
		trouble(p, "Tried to define struct member outside struct "
			"definition");
		return;
	}
	Sl(1);
	
	plus = (pez_stackitem)lookup(p, "+");
	
	name = alloc(TOK_BUF_SZ);
	token = lex(p, &p->instream, name);
	if(token != TokWord) {
		trouble(p, "Expected a word to follow 'uint8s:'");
		return;
	}
	namelen = strlen(name);
	
	Ho(Dictwordl);
	p->hptr += Dictwordl;
	buf = alloc(namelen + 2);
	memcpy(buf + 1, name, namelen);
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) { // No point in adding zero.
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle(s_exit);
	
	p->createstruct->size += S0 * sizeof(uint8_t);
	Pop;
}

prim P_int16_at(pez_instance *p)
{
	Sl(1);
	Hpc(S0);
	S0 = *(int16_t *)S0;
}

prim P_int16_bang(pez_instance *p)
{
	Sl(2);
	Hpc(S0);
	*(int16_t *)S0 = (int16_t)S1;
	Npop(2);
}

prim P_int16_size(pez_instance *p)
{
	So(1);
	Push = sizeof(int16_t);
}

prim P_int16s(pez_instance *p)
{
	Sl(1);
	S0 *= sizeof(int16_t);
}

prim P_int16_colon(pez_instance *p)
{
	pez_dictword *offsetw = (pez_dictword *)p->hptr, *w;
	pez_stackitem plus;
	int namelen, token;
	char *name, *buf;
	
	if(!p->createstruct) {
		trouble(p, "Tried to define struct member outside struct "
			"definition");
		return;
	}
	plus = (pez_stackitem)lookup(p, "+");
	
	name = alloc(TOK_BUF_SZ);
	token = lex(p, &p->instream, name);
	if(token != TokWord) {
		trouble(p, "Expected a word to follow 'int16:'");
		return;
	}
	namelen = strlen(name);
	
	// ( struct-addr -- member-addr ) The offset-adder:
	Ho(Dictwordl);
	p->hptr += Dictwordl;
	buf = alloc(namelen + 2);
	memcpy(buf + 1, name, namelen);
	offsetw->wname = buf;
	offsetw->wcode = P_nest;
	offsetw->wnext = p->dict;
	p->dict = offsetw;
	if(p->createstruct->size) { // No point in adding zero.
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle(s_exit);
	
	// ( struct-addr -- member ) The reader:
	Ho(Dictwordl);
	buf = alloc(namelen + 3);
	sprintf(buf + 1, "%s@", name);
	w = (pez_dictword *)p->hptr;
	p->hptr += Dictwordl;
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) {
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle((pez_stackitem)lookup(p, "int16@"));
	Hsingle(s_exit);
	
	// ( val struct-addr -- ) The writer:
	Ho(Dictwordl);
	w = (pez_dictword *)p->hptr;
	p->hptr += Dictwordl;
	buf = alloc(namelen + 3);
	sprintf(buf + 1, "%s!", name);
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) {
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle((pez_stackitem)lookup(p, "int16!"));
	Hsingle(s_exit);
	
	// And, finally, we add the size of a int16 to the size of the struct.
	p->createstruct->size += sizeof(int16_t);
}

prim P_int16s_colon(pez_instance *p)
{
	pez_dictword *w = (pez_dictword *)p->hptr;
	pez_stackitem plus;
	int namelen, token;
	char *name, *buf;
	
	if(!p->createstruct) {
		trouble(p, "Tried to define struct member outside struct "
			"definition");
		return;
	}
	Sl(1);
	
	plus = (pez_stackitem)lookup(p, "+");
	
	name = alloc(TOK_BUF_SZ);
	token = lex(p, &p->instream, name);
	if(token != TokWord) {
		trouble(p, "Expected a word to follow 'int16s:'");
		return;
	}
	namelen = strlen(name);
	
	Ho(Dictwordl);
	p->hptr += Dictwordl;
	buf = alloc(namelen + 2);
	memcpy(buf + 1, name, namelen);
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) { // No point in adding zero.
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle(s_exit);
	
	p->createstruct->size += S0 * sizeof(int16_t);
	Pop;
}

prim P_uint16_at(pez_instance *p)
{
	Sl(1);
	Hpc(S0);
	S0 = *(uint16_t *)S0;
}

prim P_uint16_bang(pez_instance *p)
{
	Sl(2);
	Hpc(S0);
	*(uint16_t *)S0 = (uint16_t)S1;
	Npop(2);
}

prim P_uint16_size(pez_instance *p)
{
	So(1);
	Push = sizeof(uint16_t);
}

prim P_uint16s(pez_instance *p)
{
	Sl(1);
	S0 *= sizeof(uint16_t);
}

prim P_uint16_colon(pez_instance *p)
{
	pez_dictword *offsetw = (pez_dictword *)p->hptr, *w;
	pez_stackitem plus;
	int namelen, token;
	char *name, *buf;
	
	if(!p->createstruct) {
		trouble(p, "Tried to define struct member outside struct "
			"definition");
		return;
	}
	plus = (pez_stackitem)lookup(p, "+");
	
	name = alloc(TOK_BUF_SZ);
	token = lex(p, &p->instream, name);
	if(token != TokWord) {
		trouble(p, "Expected a word to follow 'uint16:'");
		return;
	}
	namelen = strlen(name);
	
	// ( struct-addr -- member-addr ) The offset-adder:
	Ho(Dictwordl);
	p->hptr += Dictwordl;
	buf = alloc(namelen + 2);
	memcpy(buf + 1, name, namelen);
	offsetw->wname = buf;
	offsetw->wcode = P_nest;
	offsetw->wnext = p->dict;
	p->dict = offsetw;
	if(p->createstruct->size) { // No point in adding zero.
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle(s_exit);
	
	// ( struct-addr -- member ) The reader:
	Ho(Dictwordl);
	buf = alloc(namelen + 3);
	sprintf(buf + 1, "%s@", name);
	w = (pez_dictword *)p->hptr;
	p->hptr += Dictwordl;
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) {
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle((pez_stackitem)lookup(p, "uint16@"));
	Hsingle(s_exit);
	
	// ( val struct-addr -- ) The writer:
	Ho(Dictwordl);
	w = (pez_dictword *)p->hptr;
	p->hptr += Dictwordl;
	buf = alloc(namelen + 3);
	sprintf(buf + 1, "%s!", name);
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) {
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle((pez_stackitem)lookup(p, "uint16!"));
	Hsingle(s_exit);
	
	// And, finally, we add the size of a uint16 to the size of the struct.
	p->createstruct->size += sizeof(uint16_t);
}

prim P_uint16s_colon(pez_instance *p)
{
	pez_dictword *w = (pez_dictword *)p->hptr;
	pez_stackitem plus;
	int namelen, token;
	char *name, *buf;
	
	if(!p->createstruct) {
		trouble(p, "Tried to define struct member outside struct "
			"definition");
		return;
	}
	Sl(1);
	
	plus = (pez_stackitem)lookup(p, "+");
	
	name = alloc(TOK_BUF_SZ);
	token = lex(p, &p->instream, name);
	if(token != TokWord) {
		trouble(p, "Expected a word to follow 'uint16s:'");
		return;
	}
	namelen = strlen(name);
	
	Ho(Dictwordl);
	p->hptr += Dictwordl;
	buf = alloc(namelen + 2);
	memcpy(buf + 1, name, namelen);
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) { // No point in adding zero.
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle(s_exit);
	
	p->createstruct->size += S0 * sizeof(uint16_t);
	Pop;
}

prim P_int32_at(pez_instance *p)
{
	Sl(1);
	Hpc(S0);
	S0 = *(int32_t *)S0;
}

prim P_int32_bang(pez_instance *p)
{
	Sl(2);
	Hpc(S0);
	*(int32_t *)S0 = (int32_t)S1;
	Npop(2);
}

prim P_int32_size(pez_instance *p)
{
	So(1);
	Push = sizeof(int32_t);
}

prim P_int32s(pez_instance *p)
{
	Sl(1);
	S0 *= sizeof(int32_t);
}

prim P_int32_colon(pez_instance *p)
{
	pez_dictword *offsetw = (pez_dictword *)p->hptr, *w;
	pez_stackitem plus;
	int namelen, token;
	char *name, *buf;
	
	if(!p->createstruct) {
		trouble(p, "Tried to define struct member outside struct "
			"definition");
		return;
	}
	plus = (pez_stackitem)lookup(p, "+");
	
	name = alloc(TOK_BUF_SZ);
	token = lex(p, &p->instream, name);
	if(token != TokWord) {
		trouble(p, "Expected a word to follow 'int32:'");
		return;
	}
	namelen = strlen(name);
	
	// ( struct-addr -- member-addr ) The offset-adder:
	Ho(Dictwordl);
	p->hptr += Dictwordl;
	buf = alloc(namelen + 2);
	memcpy(buf + 1, name, namelen);
	offsetw->wname = buf;
	offsetw->wcode = P_nest;
	offsetw->wnext = p->dict;
	p->dict = offsetw;
	if(p->createstruct->size) { // No point in adding zero.
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle(s_exit);
	
	// ( struct-addr -- member ) The reader:
	Ho(Dictwordl);
	buf = alloc(namelen + 3);
	sprintf(buf + 1, "%s@", name);
	w = (pez_dictword *)p->hptr;
	p->hptr += Dictwordl;
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) {
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle((pez_stackitem)lookup(p, "int32@"));
	Hsingle(s_exit);
	
	// ( val struct-addr -- ) The writer:
	Ho(Dictwordl);
	w = (pez_dictword *)p->hptr;
	p->hptr += Dictwordl;
	buf = alloc(namelen + 3);
	sprintf(buf + 1, "%s!", name);
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) {
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle((pez_stackitem)lookup(p, "int32!"));
	Hsingle(s_exit);
	
	// And, finally, we add the size of a int32 to the size of the struct.
	p->createstruct->size += sizeof(int32_t);
}

prim P_int32s_colon(pez_instance *p)
{
	pez_dictword *w = (pez_dictword *)p->hptr;
	pez_stackitem plus;
	int namelen, token;
	char *name, *buf;
	
	if(!p->createstruct) {
		trouble(p, "Tried to define struct member outside struct "
			"definition");
		return;
	}
	Sl(1);
	
	plus = (pez_stackitem)lookup(p, "+");
	
	name = alloc(TOK_BUF_SZ);
	token = lex(p, &p->instream, name);
	if(token != TokWord) {
		trouble(p, "Expected a word to follow 'int32s:'");
		return;
	}
	namelen = strlen(name);
	
	Ho(Dictwordl);
	p->hptr += Dictwordl;
	buf = alloc(namelen + 2);
	memcpy(buf + 1, name, namelen);
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) { // No point in adding zero.
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle(s_exit);
	
	p->createstruct->size += S0 * sizeof(int32_t);
	Pop;
}

prim P_uint32_at(pez_instance *p)
{
	Sl(1);
	Hpc(S0);
	S0 = *(uint32_t *)S0;
}

prim P_uint32_bang(pez_instance *p)
{
	Sl(2);
	Hpc(S0);
	*(uint32_t *)S0 = (uint32_t)S1;
	Npop(2);
}

prim P_uint32_size(pez_instance *p)
{
	So(1);
	Push = sizeof(uint32_t);
}

prim P_uint32s(pez_instance *p)
{
	Sl(1);
	S0 *= sizeof(uint32_t);
}

prim P_uint32_colon(pez_instance *p)
{
	pez_dictword *offsetw = (pez_dictword *)p->hptr, *w;
	pez_stackitem plus;
	int namelen, token;
	char *name, *buf;
	
	if(!p->createstruct) {
		trouble(p, "Tried to define struct member outside struct "
			"definition");
		return;
	}
	plus = (pez_stackitem)lookup(p, "+");
	
	name = alloc(TOK_BUF_SZ);
	token = lex(p, &p->instream, name);
	if(token != TokWord) {
		trouble(p, "Expected a word to follow 'uint32:'");
		return;
	}
	namelen = strlen(name);
	
	// ( struct-addr -- member-addr ) The offset-adder:
	Ho(Dictwordl);
	p->hptr += Dictwordl;
	buf = alloc(namelen + 2);
	memcpy(buf + 1, name, namelen);
	offsetw->wname = buf;
	offsetw->wcode = P_nest;
	offsetw->wnext = p->dict;
	p->dict = offsetw;
	if(p->createstruct->size) { // No point in adding zero.
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle(s_exit);
	
	// ( struct-addr -- member ) The reader:
	Ho(Dictwordl);
	buf = alloc(namelen + 3);
	sprintf(buf + 1, "%s@", name);
	w = (pez_dictword *)p->hptr;
	p->hptr += Dictwordl;
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) {
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle((pez_stackitem)lookup(p, "uint32@"));
	Hsingle(s_exit);
	
	// ( val struct-addr -- ) The writer:
	Ho(Dictwordl);
	w = (pez_dictword *)p->hptr;
	p->hptr += Dictwordl;
	buf = alloc(namelen + 3);
	sprintf(buf + 1, "%s!", name);
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) {
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle((pez_stackitem)lookup(p, "uint32!"));
	Hsingle(s_exit);
	
	// And, finally, we add the size of a uint32 to the size of the struct.
	p->createstruct->size += sizeof(uint32_t);
}

prim P_uint32s_colon(pez_instance *p)
{
	pez_dictword *w = (pez_dictword *)p->hptr;
	pez_stackitem plus;
	int namelen, token;
	char *name, *buf;
	
	if(!p->createstruct) {
		trouble(p, "Tried to define struct member outside struct "
			"definition");
		return;
	}
	Sl(1);
	
	plus = (pez_stackitem)lookup(p, "+");
	
	name = alloc(TOK_BUF_SZ);
	token = lex(p, &p->instream, name);
	if(token != TokWord) {
		trouble(p, "Expected a word to follow 'uint32s:'");
		return;
	}
	namelen = strlen(name);
	
	Ho(Dictwordl);
	p->hptr += Dictwordl;
	buf = alloc(namelen + 2);
	memcpy(buf + 1, name, namelen);
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) { // No point in adding zero.
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle(s_exit);
	
	p->createstruct->size += S0 * sizeof(uint32_t);
	Pop;
}

prim P_int64_at(pez_instance *p)
{
	Sl(1);
	Hpc(S0);
	S0 = *(int64_t *)S0;
}

prim P_int64_bang(pez_instance *p)
{
	Sl(2);
	Hpc(S0);
	*(int64_t *)S0 = (int64_t)S1;
	Npop(2);
}

prim P_int64_size(pez_instance *p)
{
	So(1);
	Push = sizeof(int64_t);
}

prim P_int64s(pez_instance *p)
{
	Sl(1);
	S0 *= sizeof(int64_t);
}

prim P_int64_colon(pez_instance *p)
{
	pez_dictword *offsetw = (pez_dictword *)p->hptr, *w;
	pez_stackitem plus;
	int namelen, token;
	char *name, *buf;
	
	if(!p->createstruct) {
		trouble(p, "Tried to define struct member outside struct "
			"definition");
		return;
	}
	plus = (pez_stackitem)lookup(p, "+");
	
	name = alloc(TOK_BUF_SZ);
	token = lex(p, &p->instream, name);
	if(token != TokWord) {
		trouble(p, "Expected a word to follow 'int64:'");
		return;
	}
	namelen = strlen(name);
	
	// ( struct-addr -- member-addr ) The offset-adder:
	Ho(Dictwordl);
	p->hptr += Dictwordl;
	buf = alloc(namelen + 2);
	memcpy(buf + 1, name, namelen);
	offsetw->wname = buf;
	offsetw->wcode = P_nest;
	offsetw->wnext = p->dict;
	p->dict = offsetw;
	if(p->createstruct->size) { // No point in adding zero.
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle(s_exit);
	
	// ( struct-addr -- member ) The reader:
	Ho(Dictwordl);
	buf = alloc(namelen + 3);
	sprintf(buf + 1, "%s@", name);
	w = (pez_dictword *)p->hptr;
	p->hptr += Dictwordl;
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) {
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle((pez_stackitem)lookup(p, "int64@"));
	Hsingle(s_exit);
	
	// ( val struct-addr -- ) The writer:
	Ho(Dictwordl);
	w = (pez_dictword *)p->hptr;
	p->hptr += Dictwordl;
	buf = alloc(namelen + 3);
	sprintf(buf + 1, "%s!", name);
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) {
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle((pez_stackitem)lookup(p, "int64!"));
	Hsingle(s_exit);
	
	// And, finally, we add the size of a int64 to the size of the struct.
	p->createstruct->size += sizeof(int64_t);
}

prim P_int64s_colon(pez_instance *p)
{
	pez_dictword *w = (pez_dictword *)p->hptr;
	pez_stackitem plus;
	int namelen, token;
	char *name, *buf;
	
	if(!p->createstruct) {
		trouble(p, "Tried to define struct member outside struct "
			"definition");
		return;
	}
	Sl(1);
	
	plus = (pez_stackitem)lookup(p, "+");
	
	name = alloc(TOK_BUF_SZ);
	token = lex(p, &p->instream, name);
	if(token != TokWord) {
		trouble(p, "Expected a word to follow 'int64s:'");
		return;
	}
	namelen = strlen(name);
	
	Ho(Dictwordl);
	p->hptr += Dictwordl;
	buf = alloc(namelen + 2);
	memcpy(buf + 1, name, namelen);
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) { // No point in adding zero.
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle(s_exit);
	
	p->createstruct->size += S0 * sizeof(int64_t);
	Pop;
}

prim P_uint64_at(pez_instance *p)
{
	Sl(1);
	Hpc(S0);
	S0 = *(uint64_t *)S0;
}

prim P_uint64_bang(pez_instance *p)
{
	Sl(2);
	Hpc(S0);
	*(uint64_t *)S0 = (uint64_t)S1;
	Npop(2);
}

prim P_uint64_size(pez_instance *p)
{
	So(1);
	Push = sizeof(uint64_t);
}

prim P_uint64s(pez_instance *p)
{
	Sl(1);
	S0 *= sizeof(uint64_t);
}

prim P_uint64_colon(pez_instance *p)
{
	pez_dictword *offsetw = (pez_dictword *)p->hptr, *w;
	pez_stackitem plus;
	int namelen, token;
	char *name, *buf;
	
	if(!p->createstruct) {
		trouble(p, "Tried to define struct member outside struct "
			"definition");
		return;
	}
	plus = (pez_stackitem)lookup(p, "+");
	
	name = alloc(TOK_BUF_SZ);
	token = lex(p, &p->instream, name);
	if(token != TokWord) {
		trouble(p, "Expected a word to follow 'uint64:'");
		return;
	}
	namelen = strlen(name);
	
	// ( struct-addr -- member-addr ) The offset-adder:
	Ho(Dictwordl);
	p->hptr += Dictwordl;
	buf = alloc(namelen + 2);
	memcpy(buf + 1, name, namelen);
	offsetw->wname = buf;
	offsetw->wcode = P_nest;
	offsetw->wnext = p->dict;
	p->dict = offsetw;
	if(p->createstruct->size) { // No point in adding zero.
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle(s_exit);
	
	// ( struct-addr -- member ) The reader:
	Ho(Dictwordl);
	buf = alloc(namelen + 3);
	sprintf(buf + 1, "%s@", name);
	w = (pez_dictword *)p->hptr;
	p->hptr += Dictwordl;
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) {
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle((pez_stackitem)lookup(p, "uint64@"));
	Hsingle(s_exit);
	
	// ( val struct-addr -- ) The writer:
	Ho(Dictwordl);
	w = (pez_dictword *)p->hptr;
	p->hptr += Dictwordl;
	buf = alloc(namelen + 3);
	sprintf(buf + 1, "%s!", name);
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) {
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle((pez_stackitem)lookup(p, "uint64!"));
	Hsingle(s_exit);
	
	// And, finally, we add the size of a uint64 to the size of the struct.
	p->createstruct->size += sizeof(uint64_t);
}

prim P_uint64s_colon(pez_instance *p)
{
	pez_dictword *w = (pez_dictword *)p->hptr;
	pez_stackitem plus;
	int namelen, token;
	char *name, *buf;
	
	if(!p->createstruct) {
		trouble(p, "Tried to define struct member outside struct "
			"definition");
		return;
	}
	Sl(1);
	
	plus = (pez_stackitem)lookup(p, "+");
	
	name = alloc(TOK_BUF_SZ);
	token = lex(p, &p->instream, name);
	if(token != TokWord) {
		trouble(p, "Expected a word to follow 'uint64s:'");
		return;
	}
	namelen = strlen(name);
	
	Ho(Dictwordl);
	p->hptr += Dictwordl;
	buf = alloc(namelen + 2);
	memcpy(buf + 1, name, namelen);
	w->wname = buf;
	w->wcode = P_nest;
	w->wnext = p->dict;
	p->dict = w;
	if(p->createstruct->size) { // No point in adding zero.
		Hsingle(s_lit);
		Hsingle(p->createstruct->size);
		Hsingle(plus);
	}
	Hsingle(s_exit);
	
	p->createstruct->size += S0 * sizeof(uint64_t);
	Pop;
}

struct primfcn memory_primitives[] = {{"0c-int@", P_c_int_at},
{"0c-int!", P_c_int_bang},
{"0c-int-size", P_c_int_size},
{"0c-ints", P_c_ints},
{"0c-int:", P_c_int_colon},
{"0c-ints:", P_c_ints_colon},
{"0c-long@", P_c_long_at},
{"0c-long!", P_c_long_bang},
{"0c-long-size", P_c_long_size},
{"0c-longs", P_c_longs},
{"0c-long:", P_c_long_colon},
{"0c-longs:", P_c_longs_colon},
{"0c-short@", P_c_short_at},
{"0c-short!", P_c_short_bang},
{"0c-short-size", P_c_short_size},
{"0c-shorts", P_c_shorts},
{"0c-short:", P_c_short_colon},
{"0c-shorts:", P_c_shorts_colon},
{"0c-uint@", P_c_uint_at},
{"0c-uint!", P_c_uint_bang},
{"0c-uint-size", P_c_uint_size},
{"0c-uints", P_c_uints},
{"0c-uint:", P_c_uint_colon},
{"0c-uints:", P_c_uints_colon},
{"0c-ulong@", P_c_ulong_at},
{"0c-ulong!", P_c_ulong_bang},
{"0c-ulong-size", P_c_ulong_size},
{"0c-ulongs", P_c_ulongs},
{"0c-ulong:", P_c_ulong_colon},
{"0c-ulongs:", P_c_ulongs_colon},
{"0c-ushort@", P_c_ushort_at},
{"0c-ushort!", P_c_ushort_bang},
{"0c-ushort-size", P_c_ushort_size},
{"0c-ushorts", P_c_ushorts},
{"0c-ushort:", P_c_ushort_colon},
{"0c-ushorts:", P_c_ushorts_colon},
{"0c-float@", P_c_float_at},
{"0c-float!", P_c_float_bang},
{"0c-float-size", P_c_float_size},
{"0c-floats", P_c_floats},
{"0c-float:", P_c_float_colon},
{"0c-floats:", P_c_floats_colon},
{"0c-double@", P_c_double_at},
{"0c-double!", P_c_double_bang},
{"0c-double-size", P_c_double_size},
{"0c-doubles", P_c_doubles},
{"0c-double:", P_c_double_colon},
{"0c-doubles:", P_c_doubles_colon},
{"0int8@", P_int8_at},
{"0int8!", P_int8_bang},
{"0int8-size", P_int8_size},
{"0int8s", P_int8s},
{"0int8:", P_int8_colon},
{"0int8s:", P_int8s_colon},
{"0uint8@", P_uint8_at},
{"0uint8!", P_uint8_bang},
{"0uint8-size", P_uint8_size},
{"0uint8s", P_uint8s},
{"0uint8:", P_uint8_colon},
{"0uint8s:", P_uint8s_colon},
{"0int16@", P_int16_at},
{"0int16!", P_int16_bang},
{"0int16-size", P_int16_size},
{"0int16s", P_int16s},
{"0int16:", P_int16_colon},
{"0int16s:", P_int16s_colon},
{"0uint16@", P_uint16_at},
{"0uint16!", P_uint16_bang},
{"0uint16-size", P_uint16_size},
{"0uint16s", P_uint16s},
{"0uint16:", P_uint16_colon},
{"0uint16s:", P_uint16s_colon},
{"0int32@", P_int32_at},
{"0int32!", P_int32_bang},
{"0int32-size", P_int32_size},
{"0int32s", P_int32s},
{"0int32:", P_int32_colon},
{"0int32s:", P_int32s_colon},
{"0uint32@", P_uint32_at},
{"0uint32!", P_uint32_bang},
{"0uint32-size", P_uint32_size},
{"0uint32s", P_uint32s},
{"0uint32:", P_uint32_colon},
{"0uint32s:", P_uint32s_colon},
{"0int64@", P_int64_at},
{"0int64!", P_int64_bang},
{"0int64-size", P_int64_size},
{"0int64s", P_int64s},
{"0int64:", P_int64_colon},
{"0int64s:", P_int64s_colon},
{"0uint64@", P_uint64_at},
{"0uint64!", P_uint64_bang},
{"0uint64-size", P_uint64_size},
{"0uint64s", P_uint64s},
{"0uint64:", P_uint64_colon},
{"0uint64s:", P_uint64s_colon},
{NULL, (pez_wordp)0}
};