def sieveprimes(n):
    primes = [True] * (n + 1)

    primes[0] = False
    primes[1] = False

    p = 2

    while p * p <= n:
        print(p)
        if primes[p]:
            q = p
            while p * q <= n:
                primes[p * q] = False
                q = q + 1
        p = p + 1

    ret = []

    for k in range(n + 1):
        if primes[k]:
            ret.append(k)

    return ret


print(len(sieveprimes(10000000)))
