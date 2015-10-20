using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Client
{
    
    public partial class Trade_Tracker : Form
    {
        delegate void On_DataPaintdDelegate(Object o, DataGridViewRowPrePaintEventArgs e);
        private static readonly Trade_Tracker instance = new Trade_Tracker();

        public static Trade_Tracker Instance
        {
            get
            {
                return instance;
            }
        }

       
        public Trade_Tracker()
        {
            InitializeComponent();
           //this.DGV.DataSource = null;
           //this.DGV.DataSource = Global.Instance.TradeTracker;
        }
        

        private void Trade_Tracker_Load(object sender, EventArgs e)
        {
           // load_data();
            DGV.PerformLayout();
            DataSet ds = new DataSet();
            if (!File.Exists(Application.StartupPath + Path.DirectorySeparatorChar + System.DateTime.Now.Date.ToString("dddd, MMMM d, yyyy") + "tradetrack.xml"))
                return;
            Global.Instance.TradeTracker.Clear();
            ds.ReadXml(Application.StartupPath + Path.DirectorySeparatorChar + System.DateTime.Now.Date.ToString("dddd, MMMM d, yyyy") + "tradetrack.xml");
            Global.Instance.TradeTracker = ds.Tables[0];
           DGV.DataSource = Global.Instance.TradeTracker;

          
           
           
        }
        DataGridViewColumnSelector cl = null;
        private void DGV_ColumnHeaderMouseClick(object sender, DataGridViewCellMouseEventArgs e)
        {
            if (e.Button == System.Windows.Forms.MouseButtons.Right)
            {
                if (e.RowIndex == -1)
                {
                      DGV.ContextMenuStrip = null;
                      cl = new DataGridViewColumnSelector(DGV);
                }

            }   
        }

        public void load_data()
        {
            //try
            //{
            //    DataView dv = Global.Instance.OrdetTable.AsEnumerable().Where(a => a.Field<string>("Status") == orderStatus.Traded.ToString()).AsDataView();
            //    dv.Sort = "LOGTIME DESC";
            //    DGV.DataSource = dv;

            //}
            //catch (Exception ex)
            //{
            //    MessageBox.Show("Trade Book -  Funtion Name-  Load data  " + ex.Message);
            //}
            //this.DGV.Columns["LogTime"].DefaultCellStyle.Format = "H:mm:ss.fff";
        //    try
        //    {
        //       DataView dv = Global.Instance.tradeTrack.as
            
            
        //    }
        //    catch { }
            


        }

         


        private void DGV_RowPrePaint(object sender, DataGridViewRowPrePaintEventArgs e)
        {

            try
            {
               this.DGV.PerformLayout();
               if (this.DGV.InvokeRequired)
                {
                    this.DGV.Invoke(new On_DataPaintdDelegate(DGV_RowPrePaint), sender, e);
                    return;
                }
               if (Convert.ToString(this.DGV.Rows[e.RowIndex].Cells["B/S"].Value) == "BUY")
                {
                    //  DGV.Rows[e.RowIndex].DefaultCellStyle.BackColor = Color.Red;
                    this.DGV.Rows[e.RowIndex].DefaultCellStyle.ForeColor = Color.Blue;
                }
                else
                {
                    this.DGV.Rows[e.RowIndex].DefaultCellStyle.ForeColor = Color.Red;
                   
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Order Book -  Funtion Name-  DGV2_RowPrePaint  " + ex.Message);
            }
            
           
        }

     


        private void Trade_Tracker_FormClosing(object sender, FormClosingEventArgs e)
        {
            e.Cancel = true;
            this.Hide();
        }

        private void DGV_RowPrePaint_1(object sender, DataGridViewRowPrePaintEventArgs e)
        {
            try
            {
                DGV.PerformLayout();
                if (DGV.InvokeRequired)
                {
                    DGV.Invoke(new On_DataPaintdDelegate(DGV_RowPrePaint_1), sender, e);
                    return;
                }
                if (Convert.ToDouble(DGV.Rows[e.RowIndex].Cells["ACTUALPRICE"].Value)<0)
                {
                    //  DGV.Rows[e.RowIndex].DefaultCellStyle.BackColor = Color.Red;
                    DGV.Rows[e.RowIndex].DefaultCellStyle.ForeColor = Color.Blue;
                }
                else
                {
                    DGV.Rows[e.RowIndex].DefaultCellStyle.ForeColor = Color.Red;
                }
            }
            catch (Exception ex)
            {
              
            }
        }

        private void DGV_CellContentClick(object sender, DataGridViewCellEventArgs e)
        {

        }
    }
}
