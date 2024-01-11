import serial as _serial
import time   as _time

# Serial COM markers
endMarker   = '\n'
terminator  = '\r\n'

_debug_enabled = True 


class Thermocouple_api():
    """
    Commands-only object for interacting with the arduino based
    Atomic Spectra Monochromator hardware.
    
    Parameters
    ----------
    port='COM5' : str
        Name of the port to connect to.
        
    baudrate=115200 : int
        Baud rate of the connection. Must match the instrument setting.
        
    timeout = 1 : int
        How long to wait for responses before giving up (s). 
        
    """
    def __init__(self, port='COM5', baudrate=115200, timeout=.5):
                
        if not _serial:
            print('You need to install pyserial to use the Atomic Spectra Monochromator.')
            self.simulation_mode = True
        
        self.simulation_mode = False
        
        # If the port is "Simulation"
        if port=='Simulation': self.simulation_mode = True
        
        # If we have all the libraries, try connecting.
        if not self.simulation_mode:
            try:
                # Create the instrument and ensure the settings are correct.
                self.serial = _serial.Serial(port = port, baudrate = baudrate, timeout = timeout)
                
            # Something went wrong. Go into simulation mode.
            except Exception as e:
                  print('Could not open connection to "'+port+':'+'" at baudrate '+str(baudrate)+'. Entering simulation mode.')
                  print(e)
                  self.simulation_mode = True
                  self.serial = None
                
        # Container for scan data as it is dynamically acquired          
        self.scan_data = ''
                
        # Give the arduino time to run the setup loop
        _time.sleep(2)
    

    def getID(self):
        """
        Get the version of sketch currently on the arduino board.

        Returns
        -------
        str
            A string describing the arduino sketch version and compilation date.

        """
        self.write('*IDN?')
        
        return self.read()
    
    def getTemperature(self):
        """
        Get the current thermocouple temperature.

        Returns
        -------
        temp: float
            Thermocouple temperature.

        """
        self.write("THERMO:TEMP?")
        
        s = self.read()
        
        try:     temp = float(s)
        except:  temp = s
        
        return temp  
    
    def setOneshot(self):
        """
        """
        
        self.write("ONESHOT")
        
    def getConversionStatus(self):
        """
        """
        self.write('THERMO:STATUS?')
        
        return self.read()        
        
    def getThermocoupleType(self):
        """
        """
        
        self.write('THERMO:TYPE?')

        return self.read()
    
    def setThermocoupleType(self, thermocoupleType):
        """
        """
        self.write('THERMO:TYPE '+thermocoupleType)
    
    def getMode(self):
        
        self.write("THERMO:MODE?")
        
        return self.read()
    
    def setMode(self, mode):
        """            
        """
        self.write("THERMO:MODE "+mode)
     
    def getCJTemperature(self):
        
        self.write("COLDJ:TEMP? ")
        
        return self.read()
    
    
    ## Serial COM ##
    
    def write(self,raw_data):
        """
        Writes data to the serial line, formatted appropriately to be read by the monochromator.        
        
        Parameters
        ----------
        raw_data : str
            Raw data string to be sent to the arduino.
        
        Returns
        -------
        None.
        
        """
        encoded_data = (raw_data + endMarker).encode()
        self.serial.write(encoded_data) 
    
    def read(self):
        """
        Reads data from the serial line.
        
        Returns
        -------
        str
            Raw data string read from the serial line.
        """
        return self.serial.read_until(expected = terminator.encode()).decode().strip(terminator)
            
    def disconnect(self):
        """
        Disconnects the port.
        """
        if not self.simulation_mode and self.serial != None: 
            self.serial.close()
            self.serial = None
