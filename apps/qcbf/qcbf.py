from PyQt4 import QtGui
from PyQt4 import Qt
import PyQt4
import cbf
import sys
import pyxb

cbf_classes = cbf.Namespace.typeBindings()

app = QtGui.QApplication(sys.argv)

main_window = QtGui.QMainWindow()
main_window.setWindowTitle("QCBF")

menu_bar = QtGui.QMenuBar()
main_window.setMenuBar(menu_bar)

file_menu = QtGui.QMenu("&File")
file_menu.addAction("&open")
file_menu.addSeparator()
file_menu.addAction("&quit")

menu_bar.addMenu(file_menu)

splitter = QtGui.QSplitter(main_window)

document_tree = QtGui.QTreeWidget(splitter)
document_tree.setHeaderLabels(["Object", "Abstract", "Content"])
document_tree.setAcceptDrops(True)

class_tree = QtGui.QTreeWidget(splitter)
class_tree.setHeaderLabels(["Class", "Abstract"])
class_tree.setDragEnabled(True)

def insert_classes(parent, base_class, classes):
    for n in cbf_classes:
        if base_class in classes[n].__bases__:
            item = QtGui.QTreeWidgetItem()
            item.setText(0,n)
            if classes[n]._Abstract:
                item.setText(1, "X")
            else:
                item.setText(1, "-")
            if parent == 0:
                class_tree.insertTopLevelItem(0,item)
            else:
                parent.addChild(item)
            insert_classes(item, classes[n], classes)
    

insert_classes(0, pyxb.binding.basis.complexTypeDefinition, cbf_classes)
insert_classes(0, pyxb.binding.basis.simpleTypeDefinition, cbf_classes)
    
document_box = QtGui.QGroupBox("Instance")
document_box_layout = QtGui.QVBoxLayout()
document_box_layout.addWidget(document_tree)
document_box.setLayout(document_box_layout)

schema_box = QtGui.QGroupBox("Schema")
schema_box_layout = QtGui.QVBoxLayout()
schema_box_layout.addWidget(class_tree)
schema_box.setLayout(schema_box_layout)


splitter.addWidget(document_box)
splitter.addWidget(schema_box)

main_splitter = QtGui.QSplitter(Qt.Qt.Vertical)
main_splitter.addWidget(splitter)

xml_view = QtGui.QTextEdit()
main_splitter.addWidget(xml_view)

main_window.setCentralWidget(main_splitter)





main_window.show()
sys.exit(app.exec_())
