/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package imagetest;

/**
 *
 * @author Darth Scoutious
 */
import javax.imageio.ImageIO;

import com.github.sarxos.webcam.Webcam;

import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.File;
import java.util.List;

import org.zu.ardulink.Link;
import org.zu.ardulink.event.ConnectionEvent;
import org.zu.ardulink.event.ConnectionListener;
import org.zu.ardulink.event.DisconnectionEvent;
import org.zu.ardulink.protocol.IProtocol;
public class ArdulinkImageTest {
    public static void main(String[] args) {
        int pixelPercentage = 0;
    	
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
        	List<Webcam> webcamList = Webcam.getWebcams();
    		Webcam webcam = webcamList.get(0);
    		
    		//dimension array and new dimensions
    		Dimension[] views = new Dimension[4];
    		Dimension thing1 = new Dimension(800,600);
    		Dimension thing2 = new Dimension(1280,720);
    		Dimension thing3 = new Dimension(1024,768);
    		Dimension thing4 = new Dimension(1920,1080);
    		
    		// add dimensions to array
    		views[0] = thing1;
    		views[1] = thing2;
    		views[2] = thing3;
    		views[3] = thing4;
    		
    		// make sure webcam is closed so view size can be changed
    		webcam.close();
    		
    		// set the new custom views in that are in views array
    		webcam.setCustomViewSizes(views);
    		webcam.setViewSize(thing1);
    		webcam.open();
    		
    		BufferedImage image;
            //BufferedImage img = ImageIO.read(new File("src/redshirt.jpg"));
            
            List<String> portList = link.getPortList();
            if(portList != null && portList.size() > 0) {
              String port = portList.get(0);
              System.out.println("Connecting on port: " + port);
              boolean connected = link.connect(port);
              System.out.println("Connected:" + connected);
              Thread.sleep(2000);
              while(true) {
                image = webcam.getImage();
                pixelPercentage = getPixelPercent(image);
                System.out.println("Percent match: " + pixelPercentage);
                if(pixelPercentage > 70) {
                  System.out.println("YES");
                  link.sendCustomMessage("YES");
                } else {
                  System.out.println("NO");
                  link.sendCustomMessage("NO");
                }
                Thread.sleep(250);
              }
              //link.disconnect();
              //webcam.close();
            } else {
              System.out.println("No port found!");
            }
        } catch (Exception e) {
            System.out.println(e + "\nOH GOD WHY");
        }
    }

    public static int getPixelPercent(BufferedImage img)
    {
        int count = 0;
        long totalR = 0, totalG = 0, totalB = 0;
        int maxR = 0, maxG = 0, maxB = 0;
        for (int r = 0; r < img.getWidth(); r++) {
            for (int c = 0; c < img.getHeight(); c++) {
                int rgb = img.getRGB(r,c);
                int red = (rgb >> 16) & 0xFF;
                maxR = Math.max(maxR, red);
                int green = (rgb >> 8) & 0xFF;
                maxG = Math.max(maxG, green);
                int blue = rgb & 0xFF;
                maxB = Math.max(maxB, blue);
                totalR += red; totalG += green; totalB += blue;
                if (red >= 75 && red <= 135 && green >= 121 && green <= 181
                        && blue >= 118 && blue <= 178) {
                    count++;
                }
                //System.out.print("(" + red + "," + green + "," + blue + ") ");
            }
            //System.out.println();
        }
        int total = img.getWidth() * img.getHeight();
        int avgR = (int) (totalR / total);
        int avgG = (int) totalG / total;
        int avgB = (int) totalB / total;
        //System.out.println("avgR = " + avgR + ", avgG = " + avgG + ", avgB = " + avgB);
        //System.out.println("maxR = " + maxR + ", maxG = " + maxG + ", maxB = " + maxB);
        int percent = (count * 100) / total;
        return percent;
    }
}
