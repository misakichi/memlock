namespace memlock
{
    partial class MemlockForm
    {
        /// <summary>
        /// 必要なデザイナー変数です。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 使用中のリソースをすべてクリーンアップします。
        /// </summary>
        /// <param name="disposing">マネージド リソースを破棄する場合は true を指定し、その他の場合は false を指定します。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows フォーム デザイナーで生成されたコード

        /// <summary>
        /// デザイナー サポートに必要なメソッドです。このメソッドの内容を
        /// コード エディターで変更しないでください。
        /// </summary>
        private void InitializeComponent()
        {
            this.numAllocSize = new System.Windows.Forms.NumericUpDown();
            this.lstAllocList = new System.Windows.Forms.ListBox();
            this.btnAlloc = new System.Windows.Forms.Button();
            this.btnFree = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.numAllocSize)).BeginInit();
            this.SuspendLayout();
            // 
            // numAllocSize
            // 
            this.numAllocSize.Location = new System.Drawing.Point(12, 12);
            this.numAllocSize.Maximum = new decimal(new int[] {
            65536,
            0,
            0,
            0});
            this.numAllocSize.Minimum = new decimal(new int[] {
            4,
            0,
            0,
            0});
            this.numAllocSize.Name = "numAllocSize";
            this.numAllocSize.Size = new System.Drawing.Size(120, 19);
            this.numAllocSize.TabIndex = 1;
            this.numAllocSize.Value = new decimal(new int[] {
            4,
            0,
            0,
            0});
            // 
            // lstAllocList
            // 
            this.lstAllocList.FormattingEnabled = true;
            this.lstAllocList.ItemHeight = 12;
            this.lstAllocList.Location = new System.Drawing.Point(13, 38);
            this.lstAllocList.Name = "lstAllocList";
            this.lstAllocList.Size = new System.Drawing.Size(166, 196);
            this.lstAllocList.TabIndex = 2;
            // 
            // btnAlloc
            // 
            this.btnAlloc.Location = new System.Drawing.Point(139, 9);
            this.btnAlloc.Name = "btnAlloc";
            this.btnAlloc.Size = new System.Drawing.Size(40, 23);
            this.btnAlloc.TabIndex = 3;
            this.btnAlloc.Text = "alloc";
            this.btnAlloc.UseVisualStyleBackColor = true;
            this.btnAlloc.Click += new System.EventHandler(this.btnAlloc_Click);
            // 
            // btnFree
            // 
            this.btnFree.Location = new System.Drawing.Point(12, 241);
            this.btnFree.Name = "btnFree";
            this.btnFree.Size = new System.Drawing.Size(167, 23);
            this.btnFree.TabIndex = 4;
            this.btnFree.Text = "free";
            this.btnFree.UseVisualStyleBackColor = true;
            this.btnFree.Click += new System.EventHandler(this.btnFree_Click);
            // 
            // MemlockForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(233, 290);
            this.Controls.Add(this.btnFree);
            this.Controls.Add(this.btnAlloc);
            this.Controls.Add(this.lstAllocList);
            this.Controls.Add(this.numAllocSize);
            this.Name = "MemlockForm";
            this.Text = "memlock";
            ((System.ComponentModel.ISupportInitialize)(this.numAllocSize)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.NumericUpDown numAllocSize;
        private System.Windows.Forms.ListBox lstAllocList;
        private System.Windows.Forms.Button btnAlloc;
        private System.Windows.Forms.Button btnFree;
    }
}

