#!/usr/bin/perl -w


#������С��������Ϊ��ЩOBJ�ڲ�ͬ��Ŀ���棬������С�����ر��Ϊ��ȷ��ͬһ��sactter�ļ�������Ӧ��ͬ����Ŀ
#These are configured for the MT2503D

    
my $Dir_obj = "build\\GXQ03D_M2M_11C\\gprs\\MT6261o\\track" ;  


my $track_lib = "build\\GXQ03D_M2M_11C\\gprs\\MT6261o\\lib\\track.lib";
my $track_nvram_lib = "build\\GXQ03D_M2M_11C\\gprs\\MT6261o\\lib\\track_NVRAM.lib";
my $fota_lib = "track\\mtk_lib\\fota.a";

my $track_axf = "track\\track.axf";
my $tmp_lis = "track\\tmp.lis";
    
my $mak_file = "make\\GXQ03D_M2M_11C_GPRS.mak";

if (-e $mak_file)
{
    open(FH_SCAT, "<$mak_file");
    @list = <FH_SCAT>;
    close(FH_SCAT);
    for $i ( 0 .. $#list )
    {
        $line = $list[$i];
	$p1 = index( $line ,"PROJECT_SUPPORT_TRACK");
	if ( $p1 > -1)
	{
	    $p1 = index( $line ,"=") + 1;
	    $tmp = substr( $line,$p1,length($line)-$p1 );
	    $tmp =~s/^ +//;
	    $p1 = index( $tmp ," ");
	    if ( $p1 > -1 )
	    {
		$tmp = substr( $tmp,0,$p1 );
	    }
	    else
	    {
		$p1 = index( $tmp ,"\n");
		if ( $p1 > -1 )
		{
		    $tmp = substr( $tmp,0,$p1 );
		}	
	    }
	    last;
	}
	
    }
}
else
{
    die "$mak_file �����ڣ����飡";
    exit 3;
}

my $tracker_scat = "track\\Scat_Tracker_$tmp.txt" ;


if ((-e $track_lib) && (-e $track_nvram_lib) && (-e $fota_lib) )
{	
    system("armlink   --info sizes --list $tmp_lis  --nodebug --noscanlib   --output $track_axf  $track_lib(*.obj)  $track_nvram_lib(*.obj)    $fota_lib(*.o) ");       
}
		
			   
if ((-e $track_lib) && (-e $track_nvram_lib) && (-e $fota_lib) )
{	
    system("armlink   --info sizes --list $tmp_lis  --nodebug --noscanlib   --output $track_axf  $track_lib(*.obj)  $track_nvram_lib(*.obj)    $fota_lib(*.o) ");       
}
else
{
    exit 2;
}

if ((-e $track_axf) && (-e $tmp_lis) )
{	
      
}
else
{
    exit 2; #����scat�ļ�ʧ��
}



#��ȡ��ʱ����lis�ļ�������ÿ��obj�Ľ�����С
open(FH_LIS, "<$tmp_lis") or die "Can't open: $tmp_lis!\n" ;
@list = <FH_LIS>;
foreach my $line(@list)   #ѭ����ȡÿһ�е��ı�
{
    chomp $line;
    if( rindex($line,".obj") > 1) 
    {
        @tmp = split/\s+/,$line;
	$tmp[6] = $tmp[1] + $tmp[3];
        push @AoA,[ @tmp ];
    }
}



@ro_array = sort { $a->[7] cmp $b->[7]  } @AoA;
@rw_array = sort { $b->[4] <=> $a->[4]  } @AoA;
@zi_array = sort { $b->[5] <=> $a->[5]  } @AoA;


close(FH_LIS);

        my $adr_ro_load = 0x10370038;     


	my $Ext = "obj" ;
	my $i = 0;
    
    
	open(OFH, "+>$tracker_scat") or die "Can't open: $tracker_scat!\n" ;
	#�ļ���ʶͷ
	print OFH (";Add by MagicNing for RAW_DISK  & FOTA\n");
	print OFH (";\n");  
	print OFH (";v1.0   2014-10-14,  MagicNing create this file\n");
	print OFH (";v1.1   2014-11-04,  MagicNing ����RO,RW,ZI ����  \n");
	print OFH (";-------------------------------------------------------------------\n");
	print OFH (";\n");
	print OFH (";\n");  
	print OFH (";\n");
	   
	
        #���صĺ���ָ������
	$string = sprintf("0x%x", $adr_ro_load );	
	print OFH ("RO_ADDR_$i $string\n");
	print OFH ("{\n");
	print OFH ("    RO_$i +0\n"); #�ӽ���ƫ�Ʋ���ʮ�����Ʊ�ʾ����Ϊ�˺ʹ洢��ַƫ�����ֿ���������MergeSections.plƴ�Ӻϲ�bin
	$i++;
	print OFH ("    {\n");
	print OFH ("        *    ( TRACK_F_CODE, +First )\n");
	print OFH ("    }\n");
	print OFH ("}\n");
	print OFH ("\n");
	
	$adr_ro_load = 0x10370C00;
        $string = sprintf("0x%x", $adr_ro_load );
	#FOTA.a�ĺ���
	print OFH ("RO_ADDR_$i $string\n");
	print OFH ("{\n");
	print OFH ("    RO_$i +0\n");
	$i++;
	print OFH ("    {\n");
	print OFH ("        *fota.a    ( +RO,+text )\n");
	print OFH ("    }\n");
	print OFH ("}\n");
	print OFH ("\n");           
        	
	$adr_ro_load = $adr_ro_load + 0x1C00;
        
	$ro_size = 0 ;
        for $n ( 0 .. $#ro_array )
	{
	    if ( $ro_array[$n][6] > 0 )
	    {	
		if ( $ro_size == 0 )
		{
		    $string = sprintf("0x%x", $adr_ro_load );
		    #���ÿ��obj��RO����
		    print OFH ("RO_ADDR_$i $string\n");
		    print OFH ("{\n");
		    print OFH ("    RO_$i +0\n");
		    $i++;
		    print OFH ("    {\n");
		}
    
		print OFH ("        $ro_array[$n][7]  ( +RO,+text )\n");
		$ro_size += $ro_array[$n][6];
			
		if ( ($ro_size >= 0x10 ) ||  ($n == $#ro_array) )
		{
		    print OFH ("    }\n");
		    print OFH ("}\n");
		    print OFH ("\n");
		    
		    $ro_size = $ro_size + 0x40 - ($ro_size % 0x20 ) ;
		    $adr_ro_load += $ro_size;	    
		    $ro_size = 0;
		}
	    }
	    else
	    {
		$string = sprintf("0x%x", $adr_ro_load );
		#���ÿ��obj��RO����
		print OFH ("RO_ADDR_$i $string\n");
		print OFH ("{\n");
		print OFH ("    RO_$i +0\n");
		$i++;
		print OFH ("    {\n");
		
		for $a ( $n .. $#ro_array )
		{
		    print OFH ("        $ro_array[$a][7]  ( +RO,+text )\n");
		    $ro_size += $ro_array[$a][6];
		}
		
		print OFH ("    }\n");
		print OFH ("}\n");
		print OFH ("\n");
		
		if ( $ro_size > 0)
		{
		    die "������������з�0��������ĩ��!";
		    exit 2;
		}		
		$adr_ro_load = $adr_ro_load + 0x800;		
	    }
	}
	
        $i = 0;	
	my $adr_rw_load = 0x103F0038;
        my $adr_rw_exet = 0x00200000;
	$rw_size = 0 ;
        for $n ( 0 .. $#rw_array )
	{
	    if ( $rw_array[$n][4] > 0 )
	    {	
		if ( $rw_size == 0 )
		{
		    $string = sprintf("0x%x", $adr_rw_load );
	            $string2 = sprintf("0x%x", $adr_rw_exet );	
		    #���ÿ��obj��RW����
		    print OFH ("RW_ADDR_$i $string\n");
		    print OFH ("{\n");
		    print OFH ("    RW_$i $string2\n");
		    $i++;
		    print OFH ("    {\n");		    
		    $a = 0;			
		}

		print OFH ("        $rw_array[$n][7]  ( +RW, +DATA )\n");
		$rw_size += $rw_array[$n][4];
		$a++;
					
		if ( ($rw_size >= 0x80 ) ||  ($n == $#rw_array) || ($a >= 2 ) )
		{
		    print OFH ("    }\n");
		    print OFH ("}\n");
		    print OFH ("\n");
		    		    
		    $rw_size = $rw_size + 0x180 - ($rw_size % 0x100 ) ;
		    $adr_rw_load += $rw_size;
		    $adr_rw_exet += $rw_size;   #�ر�ע�⣬RW��ԭʼ������Ҫ������ڼ������ 	    
		    $rw_size = 0;
		}
	    }
	    else
	    {
		if( $rw_size > 0 )
		{
		    print OFH ("    }\n");
		    print OFH ("}\n");
		    print OFH ("\n");
		    
		    $rw_size = $rw_size + 0x100 - ($rw_size % 0x80 ) ;
		    $adr_rw_load += $rw_size;
		    $adr_rw_exet += $rw_size;   #�ر�ע�⣬RW��ԭʼ������Ҫ������ڼ������ 		    
		    $rw_size = 0;
		}
		for $a ( $n .. $#rw_array )
		{
		    if ( ($a == $n) || ( ( $a % 4 ) == 0 ) )
		    {
			$string = sprintf("0x%x", $adr_rw_load );
			$string2 = sprintf("0x%x", $adr_rw_exet );	
			#���ÿ��obj��RW����
			print OFH ("RW_ADDR_$i $string\n");
			print OFH ("{\n");
			print OFH ("    RW_$i $string2\n");
			$i++;
			print OFH ("    {\n");
		    }		    
		    print OFH ("        $rw_array[$a][7]  ( +RW, +DATA )\n");
		    $rw_size += $rw_array[$a][4];
		    $adr_rw_load = $adr_rw_load + 0x40;
		    $adr_rw_exet = $adr_rw_exet + 0x40;
		    if ( $rw_size > 0)
		    {
			die "������������з�0��������ĩ��!";
			exit 2;
		    }	
		    if ( ( $a == $#rw_array ) || ( ( $a % 4 ) == 3 )  )
		    {
			print OFH ("    }\n");
			print OFH ("}\n");
			print OFH ("\n");
		    }	
		}	
		last;
	    }
	}
	
	if ( ( $adr_rw_load + 0x100 ) > 0x103F4FFF )
	{
	    die "RW  �������������ˣ���Ҫ�޸ģ���";
	    exit 3;
	}

	
	$adr_rw_exet = $adr_rw_exet + 0x103F4F00 - $adr_rw_load ;
	$adr_rw_load = 0x103F4F00;
	$string = sprintf("0x%x", $adr_rw_load );
	$string2 = sprintf("0x%x", $adr_rw_exet );
	#��������ǿ�λ���õģ������ڴ����	      
	print OFH ("RW_ADDR_$i $string\n");
	print OFH ("{\n");
	print OFH ("    RW_$i  $string2\n");
	$i++;
	print OFH ("    {\n");
	print OFH ("        *fota.a  ( +RW, +DATA, +ZI, +BSS )\n");
	print OFH ("    }\n");
	print OFH ("}\n");
	print OFH ("\n");	
	
                  
        $i = 0;
	$adr_rw_exet = $adr_rw_exet + 0x1000 - ($adr_rw_exet % 0x1000 );
	        
	$zi_size = 0 ;
        for $n ( 0 .. $#zi_array )
	{
	    $string = sprintf("0x%x", $adr_rw_exet );
	    #���ÿ��obj��ZI����
	    print OFH ("ZI_ADDR_$i $string\n");
	    print OFH ("{\n");
	    print OFH ("    ZI_$i $string\n");
	    $i++;
	    print OFH ("    {\n");		
	    print OFH ("        $zi_array[$n][7]  ( +ZI, +BSS )\n");
	    $zi_size = $zi_array[$n][5] ;
	    print OFH ("    }\n");
	    print OFH ("}\n");
	    print OFH ("\n");
	    
	    
	    if ( $zi_size > 0 )
	    {
		$adr_rw_exet +=  $zi_size + 0x300 - ($zi_size % 0x200 ) ;
	    }
	    else
	    {
		$adr_rw_exet +=   0x200  ;
	    }
	}
	
		
	if ( ( $adr_rw_exet + 0x100 ) > 0x00300000 )
	{
	    die "ZI  �������������ˣ���Ҫ�޸ģ���";
	    exit 3;
	}
	
	#ZI ��������  ռλ��
	$adr_rw_exet = 0x002FFF00  ;
	$string2 = sprintf("0x%x", $adr_rw_exet );
	#��������ǿ�λ���õģ������ڴ����	      
	print OFH ("ZI_ADDR_$i $string2\n");
	print OFH ("{\n");
	print OFH ("    ZI_$i  $string2\n");
	$i++;
	print OFH ("    {\n");
	print OFH ("        *  ( NONE_OBJ )\n");
	print OFH ("    }\n");
	print OFH ("}\n");
	print OFH ("\n");
	
	
	
	#���ص�NVRAM����
	print OFH ("NVRAM_ADDR_0 0x103F5038\n");
	print OFH ("{\n");
	print OFH ("    NVRAM_0 +0\n");
	print OFH ("    {\n");
	print OFH ("        *      (TRACK_NVRAM_RODATA)\n");
	print OFH ("    }\n");
	print OFH ("}\n");
	print OFH ("\n");
		
	close(OFH) ;
        
	system("DEL /F /A /Q  $track_axf ") ;
	system("DEL /F /A /Q  $tmp_lis ") ;
	
exit 0;

