/*
 * waterfallView.java  
 *
 * Copyright (C) 2011 John Douyere (VK2ETA)  
 *
 * This program is distributed in the hope that it will be useful,  
 * but WITHOUT ANY WARRANTY; without even the implied warranty of  
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the  
 * GNU General Public License for more details.  
 *   
 * You should have received a copy of the GNU General Public License  
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.  
 */


package com.AndFlmsg;


import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.support.v7.widget.AppCompatImageView;
import android.util.AttributeSet;
import android.widget.ImageView;



public class waterfallView extends AppCompatImageView {
    private int xDim = 0;
    private int yDim = 100;
    private int[] Buffer;
    private double Amplitude;
    private final double audiofactor = -2.0;
    private int red, blue, green;
    public double maxvalue = 11.0;
    private int start;
    private int end;
    private int wfCwidth;

    Bitmap wfBitmap = null;
    Paint myBmPaint = null;
    Paint myTextPaint = null;


    public waterfallView(Context context) {
        super(context);
    }

    public waterfallView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
    }

    public waterfallView(Context context, AttributeSet attrs) {
        super(context, attrs);

        //Fixed 500Hz - 3000Hz range
        start = (int) (500.0 * 2048.0 / Modem.RSID_SAMPLE_RATE);
        end = 1 + (int) (3000.0 * 2048.0 / Modem.RSID_SAMPLE_RATE);
        xDim = end - start;
        Buffer = new int[xDim * yDim];
        maxvalue = AndFlmsg.mysp.getFloat("WFMAXVALUE", (float) 11.0);
        myBmPaint = new Paint();
        myBmPaint.setColor(Color.WHITE);
        myBmPaint.setStyle(Paint.Style.STROKE);
        myBmPaint.setStrokeWidth(2);
        myTextPaint = new Paint();
        myTextPaint.setTextSize(20); //for drawtext below
        myTextPaint.setColor(Color.WHITE);
        myTextPaint.setStyle(Paint.Style.FILL);
        myTextPaint.setStrokeWidth(2);
    }

    @Override
    public void onDraw(Canvas canvas) {
        int leftPos = 0;
        int topPos = 0;
        int xDimFinal = xDim;
        int yPosText;
        boolean doubleWidth = false;
        super.onDraw(canvas);

        //Future speed optimization and memory improvements (less GC)
        //Replace pixels in the bitmap with the colors in the array.
        //wfBitmap.setPixels(int[] pixels, int offset, int stride, int x, int y, int width, int height)

        //Calculate scaling factor for wider screen (orientation)
        wfCwidth = canvas.getWidth(); //returns the width of the screen NOT the waterfall object on the screen
        //wfCheight = 130; //fixed height
        //topPos = wfCheight / 2 - 200;
        //topPos = (topPos < 0 ? 0: topPos);
        //fixed
        topPos = 10;
        if (wfCwidth - xDim - 70 > xDim) {
            //We can double the waterfall width
            doubleWidth = true;
            xDimFinal = xDim * 2;
            myTextPaint.setTextSize(30);
            yPosText = 45;
        } else {
            doubleWidth = false;
            xDimFinal = xDim;
            myTextPaint.setTextSize(20);
            yPosText = 40;
        }
        leftPos = (wfCwidth - xDimFinal) / 2 - 70; //Centered and we need at least 55 dp for the buttons on the right
        leftPos = (leftPos < 1 ? 1 : leftPos);
        //Flicker issue. Always draw, even if same data
        wfBitmap = Bitmap.createBitmap(Buffer, xDim, yDim, Bitmap.Config.ARGB_8888);
        if (doubleWidth) {
            wfBitmap = Bitmap.createScaledBitmap (wfBitmap, xDim * 2, yDim, true);
        }
        //Moved to object creation section myBmPaint = new Paint();
        //Draw rectangle around waterfall
        canvas.drawRect((float) leftPos, (float) topPos, (float) leftPos + xDimFinal, (float) topPos + yDim, myBmPaint);
        //Draw the "500Hz" tick mark and text
        canvas.drawLine((float) leftPos, (float) topPos + yDim, (float) leftPos, (float) topPos + yDim + 10, myBmPaint);
        canvas.drawText("500", leftPos, yDim + yPosText, myTextPaint);
        //Draw the "1000Hz" tick mark and text
        canvas.drawLine((float) leftPos + xDimFinal / 5, (float) topPos + yDim, (float) leftPos + xDimFinal / 5, (float) topPos + yDim + 10, myBmPaint);
        canvas.drawText("1000", leftPos + xDimFinal / 5 - 20, yDim + yPosText, myTextPaint);
        //Draw the "1500Hz" tick mark and text
        canvas.drawLine((float) leftPos + xDimFinal * 2 / 5, (float) topPos + yDim, (float) leftPos + xDimFinal * 2 / 5, (float) topPos + yDim + 10, myBmPaint);
        canvas.drawText("1500", leftPos + xDimFinal * 2 / 5 - 20, yDim + yPosText, myTextPaint);
        //Draw the "2000Hz" tick mark and text
        canvas.drawLine((float) leftPos + xDimFinal * 3 / 5, (float) topPos + yDim, (float) leftPos + xDimFinal * 3 / 5, (float) topPos + yDim + 10, myBmPaint);
        canvas.drawText("2000", leftPos + xDimFinal * 3 / 5 - 20, yDim + yPosText, myTextPaint);
        //Draw the "2500Hz" tick mark and text
        canvas.drawLine((float) leftPos + xDimFinal * 4 / 5, (float) topPos + yDim, (float) leftPos + xDimFinal * 4 / 5, (float) topPos + yDim + 10, myBmPaint);
        canvas.drawText("2500", leftPos + xDimFinal * 4 / 5 - 20, yDim + yPosText, myTextPaint);
        //Draw the "3000Hz" tick mark and text
        canvas.drawLine((float) leftPos + xDimFinal, (float) topPos + yDim, (float) leftPos + xDimFinal, (float) topPos + yDim + 10, myBmPaint);
        canvas.drawText("3000", leftPos + xDimFinal - 40, yDim + yPosText, myTextPaint);
        //Draw the Signal centre listening frequency tick mark
        canvas.drawLine((float) (leftPos + xDimFinal * (Modem.frequency - 500.0) / 2500.0), 0.0f, (float) (leftPos + xDimFinal * (Modem.frequency - 500.0) / 2500.0), (float) topPos, myTextPaint);
        //Draw the updated waterfall bitmap
        canvas.drawBitmap(wfBitmap, (float) leftPos, (float) topPos, null);
        //}
        if (Modem.newAmplReady) {
            //prepare new bitmap
            //First shift one row up to make room for new values
            System.arraycopy(Buffer, 0, Buffer, 2 * xDim, xDim * (yDim - 2)); //fall down
            //Wait for new array
            //Android add dynamic centre frequency (on RSID reception)
            //			if (Modem.rsidFrequency != 0) {
            //				start = (int) ((Modem.rsidFrequency - 500.0) * 2048.0 / Modem.RSID_SAMPLE_RATE);
            //				end = 1 + (int) ((Modem.rsidFrequency + 500.0) * 2048.0 / Modem.RSID_SAMPLE_RATE);
            //			}
            //Then prepare new rows (we double the rows for visibility purposes)
            int j = -1;
            for (int i = start; i < end; i++) {
                Amplitude = Math.log(Modem.WaterfallAmpl[i]) + audiofactor;
                red = (int) (Amplitude > maxvalue * 0.5 ? 255.0 * (Amplitude / maxvalue) : 128.0 * (Amplitude / maxvalue));
                if (red > 255) red = 255;
                if (red < 0) red = 0;
                green = (int) (Amplitude > maxvalue * 0.5 ? 255.0 * (Amplitude / maxvalue) : 128.0 * (Amplitude / maxvalue));
                if (green > 255) green = 255;
                if (green < 0) green = 0;
                blue = (int) (Amplitude > maxvalue * 0.1 ? 512.0 * ((maxvalue - Amplitude) / maxvalue) : 512.0 * (Amplitude / maxvalue));
                if (blue > 255) blue = 255;
                if (blue < 0) blue = 0;
                int thispixel = 0xff000000 | (red << 16) | (green << 8) | blue;
                //Changed to single pixel
                //Buffer[++j] =  thispixel; //Double the width to allow for better viewing
                Buffer[++j] = thispixel;
            }
            //Double the thickness for visibility purposes
            System.arraycopy(Buffer, 0, Buffer, xDim, xDim);
            //Reset flag for Rx RSID method to fill another dataset
            Modem.newAmplReady = false;
        }
    }
}


