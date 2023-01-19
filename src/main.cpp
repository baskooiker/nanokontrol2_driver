#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>

#include <boost/utility/binary.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/program_options.hpp>
#include <boost/regex.hpp>
#include <boost/random.hpp>

#include "RtMidi.h"

std::string trimPort(bool doTrim, const std::string& str) {
  if (doTrim)
    return str.substr(0,str.find_last_of(" "));
  return str;
}

bool openInputPort(RtMidiIn *in, std::string port) {
  if (port.empty())
  {
    return false;
  }

  // Match full name if port contains hardware id (example: 11:0), otherwise remove the hardware id before matching
  bool doTrim = !boost::regex_match(port, boost::regex("(.+)\\s([0-9]+):([0-9]+)"));
  std::string portName;
  unsigned int i = 0, nPorts = in->getPortCount();
  for (i=0; i<nPorts; i++ ) {
    portName = in->getPortName(i);
    // std::cout << "Port name: " << portName << std::endl;
    if (trimPort(doTrim, portName) == port) {
      std::cout << "Opening input port: " << portName << std::endl;
      in->openPort(i);
      in->ignoreTypes(false, false, false);
      return true;
    }
  }
  std::cout << "Couldn't find input port: " << port << std::endl;
  return false;
}

bool openOutputPort(RtMidiOut *out, std::string port) {
  // Match full name if port contains hardware id (example: 11:0), otherwise remove the hardware id before matching
  bool doTrim = !boost::regex_match(port, boost::regex("(.+)\\s([0-9]+):([0-9]+)"));
  std::string portName;
  unsigned int i = 0, nPorts = out->getPortCount();
  for (i=0; i<nPorts; i++ ) {
    portName = out->getPortName(i);
    if (trimPort(doTrim, portName) == port) {
      std::cout << "Opening output port: " << portName << std::endl;
      out->openPort(i);
      return true;
    }
  }
  std::cout << "Couldn't find output port: " << port << std::endl;
  return false;
}

int main( )
{
    auto midiin = RtMidiIn();
    auto midiout = RtMidiOut();
    
    std::string in_name = "nanoKONTROL2";
    std::string out_name = "Midi Through";
    int sleep_time = 10;

    if (!openInputPort(&midiin, in_name))
    {
        std::cout << "Failed to open input port: " << in_name << std::endl;
        return 0xFF;
    }

    if (!openOutputPort(&midiout, out_name))
    {
        std::cout << "Failed to open output port: " << out_name << std::endl;
        return 0xFF;
    }

    std::vector<unsigned char> incomingMsg;

    while(true)
    {
        bool received_message = true;
        while (received_message)
        {
            midiin.getMessage(&incomingMsg);
            if (incomingMsg.size() > 0) 
            {
                // Check whether it's a CC message
                if (((incomingMsg)[0] & BOOST_BINARY(11110000)) == BOOST_BINARY(10110000) && incomingMsg.size() > 2) 
                    {
                        uint8_t cc = (incomingMsg)[1];
                        uint8_t value = (incomingMsg)[2];
                        std::cout << "cc: " << (int)cc << ", value: " << (int)value << std::endl;
                        // midiout->sendMessage(clockStopMessage);
                    }
            }
            else
            {
                received_message = false;
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
    }

    return 0;
}
