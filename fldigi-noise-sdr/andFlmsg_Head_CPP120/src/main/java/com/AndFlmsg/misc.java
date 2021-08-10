/*
 * misc.java  
 *   
 * Copyright (C) 2011 John Douyere (VK2ETA)  
 * Translated and adapted into Java class from Fldigi
 * as per Fldigi code from Dave Freese, W1HKJ and Stelios Bounanos, M0GLD
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

public class misc {

    public static int grayencode(int data)
//unsigned char graydecode(unsigned char data)
    {
        int bits = data;

        bits ^= data >> 1;
        bits ^= data >> 2;
        bits ^= data >> 3;
        bits ^= data >> 4;
        bits ^= data >> 5;
        bits ^= data >> 6;
        bits ^= data >> 7;

        return bits;
    }


    public static double decayavg(double average, double input, double weight) {
        if (weight <= 1.0) {
            return input;
        }
        return input * (1.0 / weight) + average * (1.0 - (1.0 / weight));
    }


    //efficient java memset for short[]
    public static void memset(short[] myarray, short j) {
        int len = myarray.length;
        if (len > 0)
            myarray[0] = j;
        for (int i = 1; i < len; i += i) {
            System.arraycopy(myarray, 0, myarray, i, ((len - i) < i) ? (len - i) : i);
        }
    }

    //memset equivalent for int[]
    public static void memset(int[] myarray, int j) {
        int len = myarray.length;
        if (len > 0)
            myarray[0] = j;
        for (int i = 1; i < len; i += i) {
            System.arraycopy(myarray, 0, myarray, i, ((len - i) < i) ? (len - i) : i);
        }
    }

    //memset equivalent for float[]
    public static void memset(float[] myarray, float j) {
        int len = myarray.length;
        if (len > 0)
            myarray[0] = j;
        for (int i = 1; i < len; i += i) {
            System.arraycopy(myarray, 0, myarray, i, ((len - i) < i) ? (len - i) : i);
        }
    }

    //memset equivalent for double[]
    public static void memset(double[] myarray, double j) {
        int len = myarray.length;
        if (len > 0)
            myarray[0] = j;
        for (int i = 1; i < len; i += i) {
            System.arraycopy(myarray, 0, myarray, i, ((len - i) < i) ? (len - i) : i);
        }
    }


}
