/*
 * This program continuously takes images from two webcams, checks if they
 * match a specific color, and sends a message to the Arduino board indicating
 * which image (if either) matches the color.
 *
 * The function getPixelPercentage can easily be replaced with any other image
 * matching function to recognize more impressive landmarks.
 */
package dualcamera;

import java.util.List;
import java.awt.*;
import java.awt.image.BufferedImage;

import com.github.sarxos.webcam.Webcam;

import org.zu.ardulink.Link;
import org.zu.ardulink.event.ConnectionEvent;
import org.zu.ardulink.event.ConnectionListener;
import org.zu.ardulink.event.DisconnectionEvent;

public class DualCamera {
    
    public static void main(String[] args) {
        int leftPixelPercentage = 0, rightPixelPercentage = 0;
    	
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
        	//Gets the list of available webcams and creates webcam objects
                //NOTE: The position of the webcams in the list will vary
                List<Webcam> webcamList = Webcam.getWebcams();
    		Webcam leftWebcam = webcamList.get(0);
                Webcam rightWebcam = webcamList.get(1);
    		
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
    		leftWebcam.close();
                rightWebcam.close();
    		
    		// set the new custom views in that are in views array
    		leftWebcam.setCustomViewSizes(views);
    		leftWebcam.setViewSize(thing1);
    		leftWebcam.open();
                
                rightWebcam.setCustomViewSizes(views);
    		rightWebcam.setViewSize(thing1);
    		rightWebcam.open();
    		
    		BufferedImage leftImage;
                BufferedImage rightImage;
            //BufferedImage img = ImageIO.read(new File("src/redshirt.jpg"));
            
            //Gets the serial port and connects to the Arduino
            List<String> portList = link.getPortList();
            if(portList != null && portList.size() > 0) {
              String port = portList.get(0);
              System.out.println("Connecting on port: " + port);
              boolean connected = link.connect(port);
              System.out.println("Connected:" + connected);
              Thread.sleep(2000); //Gives time for connection to establish
              while(true) {
                //Gets an image from each camera and tests for a color match
                leftImage = leftWebcam.getImage();
                rightImage = rightWebcam.getImage();
                leftPixelPercentage = getPixelPercent(leftImage);
                rightPixelPercentage = getPixelPercent(rightImage);
                System.out.println("Percent match  Left:" + leftPixelPercentage
                    + "  Right: " + rightPixelPercentage);
                
                //Sends message to Arduino saying which images match
                if(leftPixelPercentage > 70 && rightPixelPercentage > 70) {
                  System.out.println("BOTH");
                  link.sendCustomMessage("BOTH");
                } else if(leftPixelPercentage > 70) {
                  System.out.println("LEFT");
                  link.sendCustomMessage("LEFT");
                } else if(rightPixelPercentage > 70) {
                  System.out.println("RIGHT");
                  link.sendCustomMessage("RIGHT");
                } else {
                  System.out.println("NO");
                  link.sendCustomMessage("NO");
                }
                Thread.sleep(250);
              }
              //These functions are only needed if the program terminates
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
