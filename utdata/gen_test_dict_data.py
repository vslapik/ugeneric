import random
import string


def rand_string(N):
    charset = string.ascii_lowercase + string.digits + string.ascii_uppercase
    num = random.choice(range(1, N))
    return ''.join(random.choice(charset) for _ in range(num))


for i in xrange(500):
    # print 'DPUT(d, "%s", "%s");' % (rand_string(20), rand_string(20))
    print "%s %s" % (rand_string(20), rand_string(20))
