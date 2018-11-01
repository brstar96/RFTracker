from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

# verrices = ((1,-1,-1),(1,1,-1),
#             (-1, 1, -1), (-1,-1,-1),
#             (-1, -1, 1),(-1,1,1))
#
# def drawCube() :
#     glBegin(GL_POINTS)

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

def DrawFunc():
    glClear(GL_COLOR_BUFFER_BIT)
    glRotatef(0.01, 0.0, 0.0, 0.0)
    glutWireSphere(1, 25, 25)
    # glutWireIcosahedron(1, 25, 25)
    glFlush()

glutInit()
glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA)
glutInitWindowPosition(0, 0)
glutInitWindowSize(960, 960)
glutCreateWindow(b"first")
glutDisplayFunc(DrawFunc)
glutIdleFunc(DrawFunc)
glutMainLoop()