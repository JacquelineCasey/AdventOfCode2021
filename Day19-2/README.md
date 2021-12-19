
The plan:

For each sensor:
- Generate a set of all displacements between beacons (ordered pairs so both XY and YX) 
  (with information id-ing the becons)
- Have this set be sorted by displacements, but in a orientation blind way.

Finding potential nearby sensors (heuristically):
For all pairs of sensors:
- compare these lists (in an orientation blind way). This means set intersection,
  which is fast because the sets are sorted.
- We need at least 132 (12 * 11) items in the intersection to proceed. Otherwise,
  we KNOW the two sensors do not have 12 common nodes.

After the heuristic check, perform the (more expensive) actual check.
- For all 24 possible rotations:
  - Rotate sensor B's beacons to that rotation
    - select a node in A from the 132+ sensor pile, and compare it with each of 
      the nodes in B from the pile (do this for all 12+ nodes in the pile).
      (It may be best to only attempt check with "comprable" nodes in B).
      - shift all nodes in B by the difference between those nodes. Sort nodes by
        absolute position, and compare, looking for 12 matches.
        (Check to make sure that there are no nodes that should appear on A but don't
         and vice versa. The problem statement makes this seem optional, though).
      - If this works, link up the beacons.

Few notes / Revisions:
- We can select similar looking nodes first, and deduce the rotation from that.
  We would have to try both A[1] : B[1] + A[2] : B[2] and the other way around (A[1] : B[2] + A[2] : B[1]).
  (And if we did this, we could avoid having to store A[1]A[2] and A[2]A[1] pairs.)
  Note that a failure of this to work does not mean we have proven the sensors are
  unrelated. We would have to check more of the pairs they have in common. If we
  are worried about that taking too long, we could attempt to exclude certain pairs
  from being checked, based on the previous simulation that failed.
- One ordering for the heuristic (capturing distance, but also differentiating somewhat by
  angle to axis) would compare pairs lexicographically, but the pair is sorted in
  advance, so that (A, B, C) obeys A < B < C. Also, ignore sign. Care would be
  taken to ensure the original pair of points is preserved, and connected somehow to this
  ordering heuristic.
  - If we want the stored pair to internally remember its rotation, this cannot be
    done. Instead, we would have to allow one of the dimensions (the smallest one)
    to be negative maybe.
- Rotation about axis A requires switching axis B and C, and making one of them
  negative.
