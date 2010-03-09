import pycbf
c = pycbf.create('doc/examples/xml/python_wrap.xml')
c.step()
while not c.finished():
	c.step()

