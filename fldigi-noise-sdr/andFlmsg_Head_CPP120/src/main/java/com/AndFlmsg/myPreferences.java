/*
 * Preferences.java  
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

/**
 * @author John Douyere <vk2eta@gmail.com>
 */


import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.CheckBoxPreference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceCategory;


public class myPreferences extends PreferenceActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

/*No theme for the preferences as the checkboxes do not show on some devices. Use system default instead.	
    //Set the Activity's Theme
	int myTheme = config.getPreferenceI("APPTHEME", 0);
	switch (myTheme) {
	case 1:
	    setTheme(R.style.andFlmsgStandardDark);
	    break;
	    
	case 2:
	    setTheme(R.style.andFlmsgSmallScreen);
	    break;
	default:
	    setTheme(R.style.andFlmsgStandard);
	    break;
	}
*/
        //Start from the fixed section of the preferences
        addPreferencesFromResource(R.xml.preferences);

        //Now add the dynamic part of the preferences (mode list etc...).
        PreferenceCategory targetCategory = (PreferenceCategory) findPreference("listofmodestouse");

        for (int i = 0; i < Modem.numModes; i++) {
            //create one check box for each setting you need
            CheckBoxPreference checkBoxPreference = new CheckBoxPreference(this);
            //make sure each key is unique
            checkBoxPreference.setKey("USE" + Modem.modemCapListString[i]);
            checkBoxPreference.setTitle("Use " + Modem.modemCapListString[i]);
            //checkBoxPreference.setChecked(true);
            targetCategory.addPreference(checkBoxPreference);
        }

    }

    @Override
    protected void onResume() {
        super.onResume();

        // Use instance field for listener
        // It will not be gc'd as long as this instance is kept referenced


        AndFlmsg.splistener = new SharedPreferences.OnSharedPreferenceChangeListener() {
            public void onSharedPreferenceChanged(SharedPreferences prefs, String key) {
                // Implementation
                if (key.equals("AFREQUENCY") || key.equals("SLOWCPU") ||
                        key.startsWith("USE") || key.equals("RSID_ERRORS") ||
                        key.equals("RSIDWIDESEARCH")) {
                    AndFlmsg.RXParamsChanged = true;
                }
            }
        };

        AndFlmsg.mysp.registerOnSharedPreferenceChangeListener(AndFlmsg.splistener);

    }


}




