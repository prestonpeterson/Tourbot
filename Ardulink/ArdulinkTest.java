/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package ardulinktest;
import java.util.List;
import org.zu.ardulink.Link;
import org.zu.ardulink.event.ConnectionEvent;
import org.zu.ardulink.event.ConnectionListener;
import org.zu.ardulink.event.DisconnectionEvent;
import org.zu.ardulink.protocol.IProtocol;


/**
 *
 * @author Darth Scoutious
 */
public class ArdulinkTest {

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        Link link = Link.getDefaultInstance();

        link.addConnectionListener(new ConnectionListener() {

            @Override
            public void disconnected(DisconnectionEvent e) {
                System.out.println("Board disconnected");
            }

            @Override
            public void connected(ConnectionEvent e) {
                System.out.println("Board connected");
            }
        });
        try {
            List<String> portList = link.getPortList();
            if(portList != null && portList.size() > 0) {
              String port = portList.get(0);
              System.out.println("Connecting on port: " + port);
              boolean connected = link.connect(port);
              System.out.println("Connected:" + connected);
              Thread.sleep(2000);
              int power = IProtocol.HIGH;
//************************************************************************
// This while loop sends messages to turn on and off a digital pin
//************************************************************************
//              while(true) {
//                System.out.println("Send power:" + power);
//                link.sendPowerPinSwitch(13, power); // 5
//                if(power == IProtocol.HIGH) {
//                  power = IProtocol.LOW;
//                } else {
//                  power = IProtocol.HIGH;
//                }
//                Thread.sleep(2000);
//              }
//************************************************************************
// This while loop sends custom messages
//************************************************************************
              while(true) {
                if(power == IProtocol.HIGH) {
                  power = IProtocol.LOW;
                  System.out.println("ON");
                  link.sendCustomMessage("ON");
                } else {
                  power = IProtocol.HIGH;
                  System.out.println("OFF");
                  link.sendCustomMessage("OFF");
                }
                Thread.sleep(2000);
              }
            } else {
              System.out.println("No port found!");
            }

          } catch(Exception e) {
            e.printStackTrace();
          }
    }
    
}
