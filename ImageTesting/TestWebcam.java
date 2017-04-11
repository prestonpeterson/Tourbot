package webcamtest;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.Image;
import java.awt.event.ActionListener;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.util.List;

import javax.imageio.ImageIO;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;

import com.github.sarxos.webcam.Webcam;
import com.github.sarxos.webcam.WebcamPanel;

import java.util.Scanner;


/**
 * Example of how to take single picture.
 * 
 * @author Bartosz Firyn (SarXos)
 */
public class TestWebcam {

	public static void main(String[] args) throws IOException {

		boolean takePic = false;
		Scanner keyboard = new Scanner(System.in);
		// get default webcam and open it
		//edited
		List<Webcam> webcamList = Webcam.getWebcams();
		Webcam webcam = webcamList.get(1);
		webcam.open();
		
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
		
		// print current view size
		System.out.println(webcam.getViewSize());
		
		//change view size
		webcam.setViewSize(thing1);
		
		//view current view size
		System.out.println(webcam.getViewSize());
		
		webcam.open();

		// get image
		//BufferedImage image = webcam.getImage();

		// save image to PNG file
		//ImageIO.write(image, "PNG", new File("test.png"));
		//Image[] images = new Image[10];
		/*
		for(int i = 0; i < 10; i++) {
			BufferedImage image = webcam.getImage();
			images[i] = image;
		}*/
		//get an image from the webcam
		
		WebcamPanel panel = new WebcamPanel(webcam);
		panel.setFPSDisplayed(true);
		panel.setDisplayDebugInfo(true);
		panel.setImageSizeDisplayed(true);
		panel.setMirrored(true);
		
		JButton btnTakePicture = new JButton("Take Picture");
		panel.add(btnTakePicture);
		
		//btnTakePicture.addActionListener(this);

		JFrame window = new JFrame("Test webcam panel");
		window.add(panel);
		window.setResizable(true);
		window.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		window.pack();
		window.setVisible(true);
		
		while(!takePic) {
			int myint = keyboard.nextInt();
			if (myint > 0) {
				takePic = true;
			}
		}
		BufferedImage image = webcam.getImage();
		window.dispose();
		
		//masks to obtain values for RGB
		int redMask = 0x00FF0000;
		int greenMask = 0x0000FF00;
		int blueMask = 0x000000FF;
		
		int width = image.getWidth();
		int height = image.getHeight();
		
		System.out.println("Width: " + width);
		System.out.println("Height: " + height);
		
		//save image as a PNG
		//saves in same file as current project
		ImageIO.write(image, "PNG", new File("test2.png"));
		
		//array to hold pixel values
		int[][] arr = new int[width][height];

		//loop to get pixel values
		//pixel values are an int representing ARGB values
		// each value is 8 bits, ie R is 8 bits
		for(int i = 0; i < width; i++)
		    for(int j = 0; j < height; j++)
		        arr[i][j] = image.getRGB(i, j);
		
		System.out.println(Integer.toBinaryString(arr[0][0]));
		
		int red = (arr[0][0] & redMask) >> 16;
		System.out.println("Red: " + red);
		
		int green = (arr[0][0] & greenMask) >> 8;
		System.out.println("Green: " + green);
		
		int blue = (arr[0][0] & blueMask);
		System.out.println("Blue: " + blue);
		
		//jframe to view the image that was taken
		JFrame frame = new JFrame();
		frame.getContentPane().setLayout(new FlowLayout());
		frame.getContentPane().add(new JLabel(new ImageIcon(image)));
		frame.pack();
		frame.setVisible(true);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		webcam.close();
	}
}