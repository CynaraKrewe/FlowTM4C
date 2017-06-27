from conans import ConanFile

class ExampleUsingFlow(ConanFile):
   requires = "FlowExtra/1.2@spiessensm/stable", "FlowTM4C/1.1@spiessensm/stable"
   
   def imports(self):
      self.copy("*.h")
      self.copy("*.cpp")
      self.copy("*.a")
      self.copy(".project")
      self.copy(".cproject")
