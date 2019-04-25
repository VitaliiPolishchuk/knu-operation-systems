using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Keyboard
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        GlobalKeyboardHook gHook;
        private void Form1_Load(object sender, EventArgs e)
        {
            gHook = new GlobalKeyboardHook(); // Create a new GlobalKeyboardHook
                                              // Declare a KeyDown Event
            gHook.KeyDown += new KeyEventHandler(gHook_KeyDown);
            // Add the keys you want to hook to the HookedKeys list
            foreach (Keys key in Enum.GetValues(typeof(Keys)))
                gHook.HookedKeys.Add(key);
        }

        // Handle the KeyDown Event
        public void gHook_KeyDown(object sender, KeyEventArgs e)
        {
            textBox1.Text += ((char)e.KeyValue).ToString();
        }
        

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            gHook.unhook();
        }

        private void button1_Click_1(object sender, EventArgs e)
        {
            gHook.hook();
        }

        private void button2_Click_1(object sender, EventArgs e)
        {
            gHook.unhook();
        }
    }
}
