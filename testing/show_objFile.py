import trimesh
import sys

mesh = trimesh.load(sys.argv[1])
mesh.show()
