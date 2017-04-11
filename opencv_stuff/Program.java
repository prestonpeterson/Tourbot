/**
 * Created by Administrator on 4/6/2017.
 */

import javax.imageio.ImageIO;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.File;

public class Program {
    public static void main(String[] args) {
        try {
            BufferedImage img = ImageIO.read(new File("src/redshirt.jpg"));
            getPixelPercent(img);

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
                if (red >= 202 && green >= 50 && green <= 90 && blue > 30 && blue < 80) {
                    count++;
                }
                //System.out.print("(" + red + "," + green + "," + blue + ") ");
            }
            System.out.println();
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
