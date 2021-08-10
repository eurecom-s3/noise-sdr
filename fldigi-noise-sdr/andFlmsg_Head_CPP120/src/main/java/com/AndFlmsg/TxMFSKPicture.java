/*
 * TxMFSKPicture.java
 *
 * @author Rein Couperus
 * (Translated from Fldigi c++ by Rein Couperus)
 * 
 * Adapted to Android by John Douyere (VK2ETA)
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

public class TxMFSKPicture {

    //VK2ETA wide mfsk mode test
    double doublespacing = 1.0;
    //VK2ETA high speed modes
    private int preamble = 107;
    private int depth = 10;
    private int TXspp = 8;
    private byte[] xmtpicbuff = null;
    private double frequency;
    private double bandwidth;
    int numtones = 32;
    static final double TWOPI = 2 * Math.PI;
    static final double M_PI = Math.PI;
    double tonespacing;
    int samplerate = 8000;
    int symlen;
    double[] outbuf;
    double phaseacc;
    //    static public SoundOutput soundout;
    ViterbiEncoder Enc;
    int bitshreg;
    int bitstate;
    int symbits;
    static final int K = 7;
    static final int POLY1 = 0x6d;
    static final int POLY2 = 0x4f;
    interleave Txinlv;
    final int TX_STATE_PREAMBLE = 0;
    final int TX_STATE_START = 1;
    final int TX_STATE_DATA = 2;
    final int TX_STATE_FLUSH = 3;
    final int RX_STATE_DATA = 4;
    public boolean stopflag = false;
    int txstate = 0;
    int rxstate = 0;
    int basetone;
    static public String SendText = "";
    private static String imageModeStr = "";
    private static int imageModeIndex = 0;

    // MFSKpic receive start delay value based on a viterbi length of 45
    //  44 nulls at 8 samples per pixel
    //  88 nulls at 4 samples per pixel
    //  176 nulls at 2 samples per pixel
    private class TRACEPAIR {
        private int trace;
        private int delay;

        TRACEPAIR(int a, int b) {
            trace = a;
            delay = b;
        }
    }

    ;

    private final TRACEPAIR tracepair = new TRACEPAIR(45, 352);


    public TxMFSKPicture(int mode) {

        preamble = 107;
        depth = 10;

        frequency = config.getPreferenceI("AFREQUENCY", 1500);
        if (frequency < 500) frequency = 500;
        if (frequency > 2500) frequency = 2500;

        //volumebits = config.getPreferenceI("VOLUME",8);

        //        Enc = new ViterbiEncoder(K, POLY1, POLY2);
        //        Txinlv = new interleave (symbits, interleave.INTERLEAVE_FWD);
        //        outbuf = new double[symlen];
        imageModeIndex = Modem.getModeIndexFullList(mode);
        imageModeStr = Modem.modemCapListString[imageModeIndex];
        if (imageModeStr.equals("MFSK16")) {
            samplerate = 8000;
            symlen = 512;
            symbits = 4;
            basetone = 64;
            numtones = 16;
        } else if (imageModeStr.equals("MFSK32")) {
            samplerate = 8000;
            symlen = 256;
            symbits = 4;
            basetone = 32;
            numtones = 16;
        } else if (imageModeStr.equals("MFSK64")) {
            samplerate = 8000;
            symlen = 128;
            symbits = 4;
            basetone = 16;
            numtones = 16;
            preamble = 180;
        } else if (imageModeStr.equals("MFSK128")) {
            samplerate = 8000;
            symlen = 64;
            symbits = 4;
            basetone = 8;
            numtones = 16;
            preamble = 224;
            depth = 20;
        }

        Enc = new ViterbiEncoder(K, POLY1, POLY2);
        //Txinlv = new interleave (symbits, 0);
        Txinlv = new interleave(symbits, depth, 0);
        outbuf = new double[symlen];
        tonespacing = (double) samplerate / symlen;
        //VK2ETA test: wide mfsk mode:	 bandwidth = (numtones - 1) * tonespacing;
        bandwidth = (numtones - 1) * tonespacing;

        tx_init();
    }

    void tx_init() {
        txstate = TX_STATE_PREAMBLE;
        bitstate = 0;
    }

    void sendsymbol(int sym) {
        double f, phaseincr;
        //        double[] outDbuf = new double[symlen];
        double[] outSbuffer = new double[symlen];
        //prt(sym);
        //    	f = Main.Freq_offset - bandwidth / 2;
        f = frequency - bandwidth / 2;
        //prt(f);
        sym = misc.grayencode(sym & (numtones - 1));
        //	if (reverse)
        //		sym = (numtones - 1) - sym;

        //shift the frequency by double plus one half to centre between the two adjacent bins on the RX side
        phaseincr = TWOPI * (f + sym * tonespacing) / samplerate;

        for (int i = 0; i < symlen; i++) {
            //		outSbuffer[i] = (short) ((int)(8386560.0 * Math.cos(phaseacc)) >> volumebits);
            outSbuffer[i] = Math.cos(phaseacc); //Volume is taken care of in txModulate
            phaseacc -= phaseincr;
            if (phaseacc > M_PI)
                phaseacc -= TWOPI;
            else if (phaseacc < M_PI)
                phaseacc += TWOPI;
        }

        Modem.txModulate(outSbuffer, symlen);

    }

    void sendbit(int bit) {
        int data = Enc.encode(bit);
        for (int i = 0; i < 2; i++) {
            bitshreg = (bitshreg << 1) | ((data >> i) & 1);
            bitstate++;

            if (bitstate == symbits) {
                //VK2ETA debug fix			Txinlv.bits(bitshreg);
                bitshreg = Txinlv.bits(bitshreg);
                sendsymbol(bitshreg);
                bitstate = 0;
                bitshreg = 0;
            }
        }
    }

    void sendchar(int c) {
        String code = mfskVaricode.varienc(c);

        while (code.length() > 0) {
            if (code.charAt(0) == '1') {
                sendbit(1);
            } else {
                sendbit(0);
            }
            code = code.substring(1);
        }
        //	put_echo_char(c);
    }

    void sendidle() {
        sendchar(0);    // <NUL>
        sendbit(1);

        // extended zero bit stream
        for (int i = 0; i < 32; i++)
            sendbit(0);
    }

    void flushtx(int nbits) {
        // flush the varicode decoder at the other end
        sendbit(1);

        // flush the convolutional encoder and interleaver
        for (int i = 0; i < nbits; i++)
            sendbit(0);

        bitstate = 0;
    }


    // send prologue consisting of tracepair.delay 0's
    private void flush_xmt_filter(int n) {
        double[] outSbuf = new double[n];
        double f1 = frequency - bandwidth / 2.0;
        //double f2 = get_txfreq_woffset() + bandwidth / 2.0;
        for (int i = 0; i < n; i++) {
            outSbuf[i] = Math.cos(phaseacc);
            //phaseacc += TWOPI * (reverse ? f2 : f1) / samplerate;
            phaseacc += TWOPI * f1 / samplerate;
            if (phaseacc > TWOPI) phaseacc -= TWOPI;
        }
        //transmit (outbuf, tracepair.delay);
        Modem.txModulate(outSbuf, n);

    }

    private void send_prologue() {
        flush_xmt_filter(tracepair.delay);
    }


    static final int chunk = 128 * 8;

    void sendpic(byte[] data, int len) {
        double[] outSbuffer = new double[TXspp * chunk];
        int ptr;
        double f;
        int i, j;
        int value;

        ptr = i = 0;
        while (i < len) {
            //for (i = 0; i < len; i++) {
            //f = get_txfreq_woffset() + bandwidth * (data[i] - 128) / 256.0;
            value = data[i++];
            f = frequency + bandwidth * ((value < 0 ? value + 256 : value) - 128) / 256.0;

            for (j = 0; j < TXspp; j++) {
                outSbuffer[ptr++] = Math.cos(phaseacc);

                phaseacc += TWOPI * f / samplerate;

                if (phaseacc > M_PI)
                    phaseacc -= 2.0 * M_PI;
            }
            //Send small chuncks to the audio buffer
            if (ptr >= chunk) {
                Modem.txModulate(outSbuffer, ptr);
                ptr = 0;
            }
        }
        //Final write of buffer if any sound data left
        if (ptr > 0) Modem.txModulate(outSbuffer, ptr);

        outSbuffer = null;
    }


    void clearbits() {
        int data = Enc.encode(0);
        for (int k = 0; k < preamble; k++) {
            for (int i = 0; i < 2; i++) {
                bitshreg = (bitshreg << 1) | ((data >> i) & 1);
                bitstate++;

                if (bitstate == symbits) {
                    bitshreg = Txinlv.bits(bitshreg);
                    bitstate = 0;
                    bitshreg = 0;
                }
            }
        }
    }

    static byte[] picprologue = new byte[45 * 8];


    void txImageProcess(String textStr, byte[] img, int txPictureWidth, int txPictureHeight,
                        int txPictureTxSpeed, int txPictureColour) {
        int c;
        int xmtbytes = 0;
        //Prepare picture data first
        //Android Bitmap library: we always have a 4 bytes per pixel in RGBA order
        //   representing the bitmap picture regardless of the colour depth.
        //   Discard the Alpha and keep the RGB as the RGB values are pre-multiplied.
        int iy, ix, rowstart;
        byte value;
        if (txPictureColour != 0) {
            //RGB = 3 bytes per pixel
            xmtbytes = txPictureWidth * txPictureHeight * 3;
            xmtpicbuff = new byte[xmtbytes];
            //unsigned char *outbuf = xmtpicbuff;
            for (iy = 0; iy < txPictureHeight; iy++) {
                rowstart = iy * txPictureWidth;
                for (ix = 0; ix < txPictureWidth; ix++) {
                    //Skip Alpha bytes and change from signed to unsigned char
                    xmtpicbuff[(rowstart * 3 + ix)] = img[(rowstart + ix) * 4];
                    xmtpicbuff[(rowstart * 3 + ix + txPictureWidth)] = img[(rowstart + ix) * 4 + 1];
                    xmtpicbuff[(rowstart * 3 + ix + txPictureWidth + txPictureWidth)] =
                            img[(rowstart + ix) * 4 + 2];
                }
            }
        } else {
            //Grey-Scale = single byte per pixel
            xmtbytes = txPictureWidth * txPictureHeight;
            xmtpicbuff = new byte[xmtbytes];
            int greyTotal;
            for (iy = 0; iy < txPictureHeight; iy++) {
                rowstart = iy * txPictureWidth;
                for (ix = 0; ix < txPictureWidth; ix++) {
                    value = img[(rowstart + ix) * 4];
                    greyTotal = 31 * (value < 0 ? value + 256 : value);
                    value = img[(rowstart + ix) * 4 + 1];
                    greyTotal += 61 * (value < 0 ? value + 256 : value);
                    value = img[(rowstart + ix) * 4 + 2];
                    greyTotal += 8 * (value < 0 ? value + 256 : value);
                    xmtpicbuff[rowstart + ix] = (byte) (greyTotal / 100);
                }
            }
        }

        TXspp = txPictureTxSpeed;
        //add picture text preamble (e.g. Sending Pic:115x204p2;)
        textStr += "\nSending Pic:" + txPictureWidth + "x" + txPictureHeight + (txPictureColour == 0 ? "" : "C") +
                (txPictureTxSpeed == 8 ? "" : "p" + txPictureTxSpeed) + ";";
        //Preamble
        clearbits();
        for (int i = 0; i < preamble / 3; i++)
            sendbit(0);
        //Start
        sendchar('\r');
        //data
        char[] charlist = textStr.toCharArray();
        for (int i = 0; i < textStr.length(); i++) {
            //Catch the stopTX flag at this point
            if (!Modem.stopTX) {
                c = charlist[i];
                sendchar(c);
            }
        }
        flushtx(preamble);
        //send_prologue();
        // 176 samples (old method)
        for (int xx = 0; xx < picprologue.length; xx++) picprologue[xx] = 0; // 44 * 8 / TXspp);
        int samples = 44 * 8 / TXspp;
        //Quick fix to correct the MFSK128 difference in preamble
        if (imageModeStr.equals("MFSK128")) {
            samples = 4 * 8 / TXspp;
        }
        sendpic(picprologue, samples);
        //Send picture
        sendpic(xmtpicbuff, xmtbytes);
        flushtx(preamble);
        //Clear the transmit buffer
        if (xmtpicbuff != null) {
            xmtpicbuff = null;
        }

    }

}
