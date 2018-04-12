class GenericsPrettyPrinter:
    ''' Pretty printing of ugeneric_t instances.
    '''
    def __init__(self, value):
        #print(str(value))
        self.v = value

    def to_string (self):
        t = self.v['t']
        v = self.v['v']
        #print(str(t))
        #print(str(v))

        if t['type'] >= 11: # magic constant comes from generic.h
            return "G_MEMCHUNK{.data = %s, .size = %s}" % (v['ptr'], t['memchunk_size'] - 11)
        else:
            return str(t['type'])[:-2] + "{" + {
                "G_ERROR_T": str(v['err']),
                "G_NULL_T":  "",
                "G_PTR_T":   str(v['ptr']),
                "G_STR_T":   str(v['str']),
                "G_CSTR_T":  str(v['cstr']),
                "G_INT_T":   str(v['integer']),
                "G_REAL_T":  str(v['real']),
                "G_SIZE_T":  str(v['size']),
                "G_BOOL_T":  str(v['boolean']),
                "G_VECTOR_T": str(v['ptr']),
                "G_DICT_T":  str(v['ptr']),
            }.get(str(t['type']), "unknown") + "}"

def print_ugeneric_t(value):
    if str(value.type) == 'ugeneric_t':
        return GenericsPrettyPrinter(value)
    return None

gdb.pretty_printers.append(print_ugeneric_t)
