from conans import ConanFile

class ExampleUsingFlow(ConanFile):
   requires = "FlowExtra/1.0@spiessensm/stable", "TM4C-usblib/2.1.3.156a@spiessensm/stable"
   
   def imports(self):
      self.copy("*.h")
      self.copy("*.cpp")
      self.copy("*.a")
      self.copy(".project")
      self.copy(".cproject")
