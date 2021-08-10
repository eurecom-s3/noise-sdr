/* --------------------------------------------------------------------------
 * interleave.java --  MFSK (de)interleaver
 * 
 * Adapted into Java classes by John Douyere (VK2ETA) from Fldigi C++ code written by Dave Freese, W1HKJ
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * -----------------------------------------------------------------------
 */


package com.AndFlmsg;


public class interleave {
    public static final int INTERLEAVE_FWD = 0;
    public static final int INTERLEAVE_REV = 1;

    protected int size;
    protected int depth;
    protected int direction;
    int[] table;

    int tab(int i, int j, int k) {
        return ((size * size * i) + (size * j) + k);
    }


    public interleave(int _size, int dir) {
        size = _size;
        if (size == -1) { // dominoEX interleaver
            size = 4;
            depth = 4;
            //BPSK+FEC+interleaver. First digit is size, then number of concatenated square interleavers
        } else if (size == -220) { // BPSK FEC + Interleaver 2x2x20
            size = 2;
            depth = 20;
        } else if (size == -240) { // BPSK FEC + Interleaver 2x2x40
            size = 2;
            depth = 40;
        } else if (size == -280) { // BPSK FEC + Interleaver 2x2x80
            size = 2;
            depth = 80;
        } else if (size == -2160) { // BPSK FEC + Interleaver 2x2x160
            size = 2;
            depth = 160;
        } else if (size == 420) { // THOR/MFSK Interleaver 4x4x20
            size = 4;
            depth = 20;
        } else if (size == 440) { // THOR/MFSK Interleaver 4x4x40
            size = 4;
            depth = 40;
        } else if (size == 480) { // THOR/MFSK Interleaver 4x4x80
            size = 4;
            depth = 80;
        } else if (size == 5)
            depth = 5;
        else
            depth = 10;
        direction = dir;
        table = new int[depth * size * size];
        misc.memset(table, 0);
    }


    public interleave(int _size, int _depth, int dir) {
        size = _size;
        depth = _depth;
        direction = dir;
        table = new int[depth * size * size];
        //flush();
        misc.memset(table, 0);
    }


    void symbols(int[] psyms) {
        int i, j, k;
        int sizeSQ = size * size;
        int thisptr;

        for (k = 0; k < depth; k++) {
            for (i = 0; i < size; i++)
                for (j = 0; j < size - 1; j++) {
//					int tab(int i, int j, int k) {
//					return ((size * size * i) + (size * j) + k);
//					table[tab(k, i, j)] = table[tab(k, i, j + 1)];
                    thisptr = k * sizeSQ + i * size + j;
                    table[thisptr] = table[thisptr + 1];
                }
            for (i = 0; i < size; i++)
//				table[tab(k, i, size - 1)] = psyms[i];
                table[k * sizeSQ + i * size + size - 1] = psyms[i];

            for (i = 0; i < size; i++) {
                if (direction == INTERLEAVE_FWD)
//					psyms[i] = table[tab(k, i, size - i - 1)];
                    psyms[i] = table[k * sizeSQ + i * size + size - i - 1];
                else
//					psyms[i] = table[tab(k, i, i)];
                    psyms[i] = table[k * sizeSQ + i * size + i];
            }
        }
    }


    int bits(int pbits) {
        int[] syms = new int[size];
        int i;

        for (i = 0; i < size; i++)
            syms[i] = (pbits >> (size - i - 1)) & 1;

        symbols(syms);

        pbits = 0;
        for (i = 0; i < size; i++)
            pbits = (pbits << 1) | syms[i];

        return pbits;
    }

}
