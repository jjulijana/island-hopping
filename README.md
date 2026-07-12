# island-hopping
You are a contractor for the small independent nation of Microisles, which is far out in the Pacific ocean, and made up of a large number of islands. The islanders travel between islands on boats, but the government has hired you to design a set of bridges that would connect all the islands together. However, they want to do this at a minimum cost.


## Running app

```
cmake -S . -B build
cmake --build build
```

```
./build/island-hopping
```
 or
```
./build/island-hopping --console
```
 or
```
./build/island-hopping --console "path.txt"
```


## Testing

`10cases.txt` (n=10) covers:

- m=1 — single island, cost should be exactly 0
- m=2 — two islands very close together (tests decimal precision)
- 3-point right triangle with negative coordinates
- Unit square (m=4) — MST should be 3 sides = 3.000
- Collinear points on a horizontal line (m=5) — MST is just the line segments
- Collinear points on a vertical line (m=4) — same x-coordinate
- Random cluster (m=20) — general case, no special structure
- Two well-separated clusters (m=10) — tests that MST correctly bridges distant groups with one long edge
- Extreme boundary coordinates (m=6) — uses the full ±1000 range
- The official sample case 2 — included as a self-check against Kattis's published answer

`stress_test.txt`:
- bonus stress test at the maximum size (m=750, the upper limit)