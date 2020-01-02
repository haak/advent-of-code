ans = 0
for s in range(245182, 790572 + 1):
    x = str(s)
    if any(d*2 in x and d*3 not in x for d in '0123456789') and all(l<=r for l,r in zip(x[::], x[1::])):
        ans += 1
print(ans)