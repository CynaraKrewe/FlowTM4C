from conans import ConanFile
from conans.tools import download, unzip
import os
import shutil

class FlowTM4C(ConanFile):
	name = "FlowTM4C"
	version = "1.2"
	description = """Flow compatible drivers for the Texas Intruments TM4C129 microcontroller family."""
	url = "https://github.com/CynaraKrewe/FlowTM4C"
	license = "MIT"
	author = "Mathias Spiessens"
	build_policy = "missing"
	requires = "Flow/1.3@spiessensm/stable", "TM4C-usblib/2.1.4.178@spiessensm/stable"
	
	def source(self):
		download("https://github.com/CynaraKrewe/FlowTM4C/archive/v1.2.zip", "FlowTM4C-1.2.zip")
		unzip("FlowTM4C-1.2.zip")
		shutil.move("FlowTM4C-1.2", "FlowTM4C")
		os.unlink("FlowTM4C-1.2.zip")

	def build(self):
		self.output.info("Nothing to build, this package provides sources.")

	def package(self):
		self.copy("*.h", "include/flow/tm4c", "FlowTM4C/include/flow/tm4c")
		self.copy("*.cpp", "source/flow/tm4c", "FlowTM4C/source/flow/tm4c")
