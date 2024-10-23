using System;
using System.Windows.Forms;
using System.IO.Ports;
namespace lala2
{
    public partial class Form1 : Form
    {
        SerialPort port;
        public Form1()
        {
            InitializeComponent();
            this.FormClosed += new FormClosedEventHandler(Form1_FormClosed);
            if (port == null)
            {
                port = new SerialPort("COM5", 9600);//Set your board COM
                port.Open();
            }
        }
        void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            if (port != null && port.IsOpen)
            {
                port.Close();
            }
        }
        private void button1_Click(object sender, EventArgs e)
        {
            PortWrite("1");  // Відправляємо "1" для увімкнення світлодіода
            label1.Text = "LED is ON";  // Оновлюємо текст Label
        }

        private void button2_Click(object sender, EventArgs e)
        {
            PortWrite("0");  // Відправляємо "0" для вимкнення світлодіода
            label1.Text = "LED is OFF";  // Оновлюємо текст Label
        }

        private void PortWrite(string message)
        {
            port.Write(message);
        }
    }
}