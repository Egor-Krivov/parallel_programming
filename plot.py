from mpl_toolkits.mplot3d import axes3d
import matplotlib.pyplot as plt
import numpy as np

grid = []
out = ''
while True:
    try:
        line = input()
        if line == "":
            print(input())
            print(input())
            break;
        else:
            time_line = [float(s) for s in line.split()]
            grid.append(time_line)
    except EOFError:
        break
grid = np.array(list(reversed(grid)))
    
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
X, Y = np.linspace(0.0, 1.0, grid.shape[1]), np.linspace(0.0, 1.0, grid.shape[0])
X, Y = np.meshgrid(X, Y)
Z = grid
ax.axis()
ax.plot_wireframe(X, Y, Z, rstride=grid.shape[0] // 20 + 1, cstride=grid.shape[1] // 20 + 1)
ax.set_xlabel('T')
ax.set_ylabel('X')


plt.show()