from conans import ConanFile

class Flow(ConanFile):
   requires = "Flow/1.5@spiessensm/stable", "TM4C-usblib/2.1.4.178@spiessensm/stable"
   
   def imports(self):
      self.copy("*.h")
      self.copy("*.cpp")
      self.copy("*.a")
