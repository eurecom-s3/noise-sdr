/* ----------------------------------------------------------------------------
 * ViterbiEncoder.java  --  Viterbi encoder
 *
 * Transcoded from Fldigi C++ to Java by John Douyere (VK2ETA)
 *
 * Adapted from code contained in gmfsk source code distribution.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ----------------------------------------------------------------------------
 */


package com.AndFlmsg;


public class ViterbiEncoder {

    //	private	int *output;
    private int[] output;
    //	private	unsigned int shreg;
    private int shreg = 0;
    //	private	unsigned int shregmask;
    private int shregmask;


    //#include <iostream>
    public ViterbiEncoder(int k, int poly1, int poly2) {
        int size = 1 << k;	/* size of the output table */

        output = new int[size];
// output contains 2 bits in positions 0 and 1 describing the state machine
// for each bit delay, ie: for k = 7 there are 128 possible state pairs.
// the modulo-2 addition for polynomial 1 is in bit 0
// the modulo-2 addition for polynomial 2 is in bit 1
// the allowable state outputs are 0, 1, 2 and 3
        for (int i = 0; i < size; i++) {
            // Note: Parity function. Return one if `w' has odd number of ones, zero otherwise.
            output[i] = (Integer.bitCount(poly1 & i) % 2) | ((Integer.bitCount(poly2 & i) % 2) << 1);
        }
        shreg = 0;
        shregmask = size - 1;
    }


    public int encode(int bit) {
//	shreg = (shreg << 1) | !!bit;
        shreg = (shreg << 1) | (bit != 0 ? 1 : 0);

        return output[shreg & shregmask];
    }

}