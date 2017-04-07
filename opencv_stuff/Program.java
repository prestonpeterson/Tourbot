/**
 * Created by Administrator on 4/6/2017.
 */

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;

import javax.imageio.ImageIO;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.awt.image.DataBufferByte;
import java.io.File;

import org.opencv.imgproc.Imgproc;
import org.opencv.imgproc.Imgproc.*;

import static org.opencv.imgcodecs.Imgcodecs.CV_LOAD_IMAGE_COLOR;
import static org.opencv.imgcodecs.Imgcodecs.imread;

public class Program {
    public static void main(String[] args) {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
        try {
            BufferedImage img = ImageIO.read(new File("src/test1.png"));
            Mat img = imread("lena.png", CV_LOAD_IMAGE_COLOR); // TODO: USE IMREAD
            int height = img.getHeight();
            int width = img.getWidth();
            Mat mat1 = new Mat(height,width,CvType.CV_32FC3);
            byte[] data = ((DataBufferByte) img.getRaster().getDataBuffer()).getData();
            mat1.put(0, 0, data);

            BufferedImage img2 = ImageIO.read(new File("src/test2.png"));
            int h = img2.getHeight();
            int w = img2.getWidth();
            Mat mat2 = new Mat(h,w,CvType.CV_32FC3);
            byte[] data2 = ((DataBufferByte) img2.getRaster().getDataBuffer()).getData();
            mat2.put(0, 0, data2);

            double result = Imgproc.compareHist(mat1, mat2, Imgproc.CV_COMP_CORREL);
            System.out.println("Result = " + result);

        } catch (Exception e) {
            System.out.println(e + "\nOH GOD WHY");
        }


    }
}
