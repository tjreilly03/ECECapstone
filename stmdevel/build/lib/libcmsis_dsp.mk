ROOTDIR = ..
include $(ROOTDIR)/Makefile.inc

# Default Architecture (normally overwridden by make command line)
ARCH = STM32L476xx

TARGET=libcmsis_dsp_$(ARCH).a

RAWOBJS= arm_abs_f32.o \
    arm_abs_q15.o \
    arm_abs_q31.o \
    arm_abs_q7.o \
    arm_add_f32.o \
    arm_add_q15.o \
    arm_add_q31.o \
    arm_add_q7.o \
    arm_dot_prod_f32.o \
    arm_dot_prod_q15.o \
    arm_dot_prod_q31.o \
    arm_dot_prod_q7.o \
    arm_mult_f32.o \
    arm_mult_q15.o \
    arm_mult_q31.o \
    arm_mult_q7.o \
    arm_negate_f32.o \
    arm_negate_q15.o \
    arm_negate_q31.o \
    arm_negate_q7.o \
    arm_offset_f32.o \
    arm_offset_q15.o \
    arm_offset_q31.o \
    arm_offset_q7.o \
    arm_scale_f32.o \
    arm_scale_q15.o \
    arm_scale_q31.o \
    arm_scale_q7.o \
    arm_shift_q15.o \
    arm_shift_q31.o \
    arm_shift_q7.o \
    arm_sub_f32.o \
    arm_sub_q15.o \
    arm_sub_q31.o \
    arm_sub_q7.o \
    arm_cmplx_conj_f32.o \
    arm_cmplx_conj_q15.o \
    arm_cmplx_conj_q31.o \
    arm_cmplx_dot_prod_f32.o \
    arm_cmplx_dot_prod_q15.o \
    arm_cmplx_dot_prod_q31.o \
    arm_cmplx_mag_f32.o \
    arm_cmplx_mag_q15.o \
    arm_cmplx_mag_q31.o \
    arm_cmplx_mag_squared_f32.o \
    arm_cmplx_mag_squared_q15.o \
    arm_cmplx_mag_squared_q31.o \
    arm_cmplx_mult_cmplx_f32.o \
    arm_cmplx_mult_cmplx_q15.o \
    arm_cmplx_mult_cmplx_q31.o \
    arm_cmplx_mult_real_f32.o \
    arm_cmplx_mult_real_q15.o \
    arm_cmplx_mult_real_q31.o \
    arm_pid_init_f32.o \
    arm_pid_init_q15.o \
    arm_pid_init_q31.o \
    arm_pid_reset_f32.o \
    arm_pid_reset_q15.o \
    arm_pid_reset_q31.o \
    arm_sin_cos_f32.o \
    arm_sin_cos_q31.o \
    arm_cos_f32.o \
    arm_cos_q15.o \
    arm_cos_q31.o \
    arm_sin_f32.o \
    arm_sin_q15.o \
    arm_sin_q31.o \
    arm_sqrt_q15.o \
    arm_sqrt_q31.o \
    arm_biquad_cascade_df1_32x64_init_q31.o \
    arm_biquad_cascade_df1_32x64_q31.o \
    arm_biquad_cascade_df1_f32.o \
    arm_biquad_cascade_df1_fast_q15.o \
    arm_biquad_cascade_df1_fast_q31.o \
    arm_biquad_cascade_df1_init_f32.o \
    arm_biquad_cascade_df1_init_q15.o \
    arm_biquad_cascade_df1_init_q31.o \
    arm_biquad_cascade_df1_q15.o \
    arm_biquad_cascade_df1_q31.o \
    arm_biquad_cascade_df2T_f32.o \
    arm_biquad_cascade_df2T_f64.o \
    arm_biquad_cascade_df2T_init_f32.o \
    arm_biquad_cascade_df2T_init_f64.o \
    arm_biquad_cascade_stereo_df2T_f32.o \
    arm_biquad_cascade_stereo_df2T_init_f32.o \
    arm_conv_f32.o \
    arm_conv_fast_opt_q15.o \
    arm_conv_fast_q15.o \
    arm_conv_fast_q31.o \
    arm_conv_opt_q15.o \
    arm_conv_opt_q7.o \
    arm_conv_partial_f32.o \
    arm_conv_partial_fast_opt_q15.o \
    arm_conv_partial_fast_q15.o \
    arm_conv_partial_fast_q31.o \
    arm_conv_partial_opt_q15.o \
    arm_conv_partial_opt_q7.o \
    arm_conv_partial_q15.o \
    arm_conv_partial_q31.o \
    arm_conv_partial_q7.o \
    arm_conv_q15.o \
    arm_conv_q31.o \
    arm_conv_q7.o \
    arm_correlate_f32.o \
    arm_correlate_fast_opt_q15.o \
    arm_correlate_fast_q15.o \
    arm_correlate_fast_q31.o \
    arm_correlate_opt_q15.o \
    arm_correlate_opt_q7.o \
    arm_correlate_q15.o \
    arm_correlate_q31.o \
    arm_correlate_q7.o \
    arm_fir_decimate_f32.o \
    arm_fir_decimate_fast_q15.o \
    arm_fir_decimate_fast_q31.o \
    arm_fir_decimate_init_f32.o \
    arm_fir_decimate_init_q15.o \
    arm_fir_decimate_init_q31.o \
    arm_fir_decimate_q15.o \
    arm_fir_decimate_q31.o \
    arm_fir_f32.o \
    arm_fir_fast_q15.o \
    arm_fir_fast_q31.o \
    arm_fir_init_f32.o \
    arm_fir_init_q15.o \
    arm_fir_init_q31.o \
    arm_fir_init_q7.o \
    arm_fir_interpolate_f32.o \
    arm_fir_interpolate_init_f32.o \
    arm_fir_interpolate_init_q15.o \
    arm_fir_interpolate_init_q31.o \
    arm_fir_interpolate_q15.o \
    arm_fir_interpolate_q31.o \
    arm_fir_lattice_f32.o \
    arm_fir_lattice_init_f32.o \
    arm_fir_lattice_init_q15.o \
    arm_fir_lattice_init_q31.o \
    arm_fir_lattice_q15.o \
    arm_fir_lattice_q31.o \
    arm_fir_q15.o \
    arm_fir_q31.o \
    arm_fir_q7.o \
    arm_fir_sparse_f32.o \
    arm_fir_sparse_init_f32.o \
    arm_fir_sparse_init_q15.o \
    arm_fir_sparse_init_q31.o \
    arm_fir_sparse_init_q7.o \
    arm_fir_sparse_q15.o \
    arm_fir_sparse_q31.o \
    arm_fir_sparse_q7.o \
    arm_iir_lattice_f32.o \
    arm_iir_lattice_init_f32.o \
    arm_iir_lattice_init_q15.o \
    arm_iir_lattice_init_q31.o \
    arm_iir_lattice_q15.o \
    arm_iir_lattice_q31.o \
    arm_lms_f32.o \
    arm_lms_init_f32.o \
    arm_lms_init_q15.o \
    arm_lms_init_q31.o \
    arm_lms_norm_f32.o \
    arm_lms_norm_init_f32.o \
    arm_lms_norm_init_q15.o \
    arm_lms_norm_init_q31.o \
    arm_lms_norm_q15.o \
    arm_lms_norm_q31.o \
    arm_lms_q15.o \
    arm_lms_q31.o \
    arm_mat_add_f32.o \
    arm_mat_add_q15.o \
    arm_mat_add_q31.o \
    arm_mat_cmplx_mult_f32.o \
    arm_mat_cmplx_mult_q15.o \
    arm_mat_cmplx_mult_q31.o \
    arm_mat_init_f32.o \
    arm_mat_init_q15.o \
    arm_mat_init_q31.o \
    arm_mat_inverse_f32.o \
    arm_mat_inverse_f64.o \
    arm_mat_mult_f32.o \
    arm_mat_mult_fast_q15.o \
    arm_mat_mult_fast_q31.o \
    arm_mat_mult_q15.o \
    arm_mat_mult_q31.o \
    arm_mat_scale_f32.o \
    arm_mat_scale_q15.o \
    arm_mat_scale_q31.o \
    arm_mat_sub_f32.o \
    arm_mat_sub_q15.o \
    arm_mat_sub_q31.o \
    arm_mat_trans_f32.o \
    arm_mat_trans_q15.o \
    arm_mat_trans_q31.o \
    arm_max_f32.o \
    arm_max_q15.o \
    arm_max_q31.o \
    arm_max_q7.o \
    arm_mean_f32.o \
    arm_mean_q15.o \
    arm_mean_q31.o \
    arm_mean_q7.o \
    arm_min_f32.o \
    arm_min_q15.o \
    arm_min_q31.o \
    arm_min_q7.o \
    arm_power_f32.o \
    arm_power_q15.o \
    arm_power_q31.o \
    arm_power_q7.o \
    arm_rms_f32.o \
    arm_rms_q15.o \
    arm_rms_q31.o \
    arm_std_f32.o \
    arm_std_q15.o \
    arm_std_q31.o \
    arm_var_f32.o \
    arm_var_q15.o \
    arm_var_q31.o \
    arm_copy_f32.o \
    arm_copy_q15.o \
    arm_copy_q31.o \
    arm_copy_q7.o \
    arm_fill_f32.o \
    arm_fill_q15.o \
    arm_fill_q31.o \
    arm_fill_q7.o \
    arm_float_to_q15.o \
    arm_float_to_q31.o \
    arm_float_to_q7.o \
    arm_q15_to_float.o \
    arm_q15_to_q31.o \
    arm_q15_to_q7.o \
    arm_q31_to_float.o \
    arm_q31_to_q15.o \
    arm_q31_to_q7.o \
    arm_q7_to_float.o \
    arm_q7_to_q15.o \
    arm_q7_to_q31.o \
    arm_bitreversal.o \
    arm_bitreversal2.o \
    arm_cfft_f32.o \
    arm_cfft_q15.o \
    arm_cfft_q31.o \
    arm_cfft_radix2_f32.o \
    arm_cfft_radix2_init_f32.o \
    arm_cfft_radix2_init_q15.o \
    arm_cfft_radix2_init_q31.o \
    arm_cfft_radix2_q15.o \
    arm_cfft_radix2_q31.o \
    arm_cfft_radix4_f32.o \
    arm_cfft_radix4_init_f32.o \
    arm_cfft_radix4_init_q15.o \
    arm_cfft_radix4_init_q31.o \
    arm_cfft_radix4_q15.o \
    arm_cfft_radix4_q31.o \
    arm_cfft_radix8_f32.o \
    arm_dct4_f32.o \
    arm_dct4_init_f32.o \
    arm_dct4_init_q15.o \
    arm_dct4_init_q31.o \
    arm_dct4_q15.o \
    arm_dct4_q31.o \
    arm_rfft_f32.o \
    arm_rfft_fast_f32.o \
    arm_rfft_fast_init_f32.o \
    arm_rfft_init_f32.o \
    arm_rfft_init_q15.o \
    arm_rfft_init_q31.o \
    arm_rfft_q15.o \
    arm_rfft_q31.o \
    arm_common_tables.o \
    arm_const_structs.o

OBJS = $(addsuffix .$(ARCH), $(RAWOBJS))

# Suppress the "Comparison between signed and unsigned types" warnings.  MANY of the 
# cmsis supplied source files routinely compare unsigned and signed ints.  
#    Sorry...  Not my code!
CFLAGS = $(PRJCFLAGS)  -D$(ARCH) -Wno-sign-compare -O3 -I. -I$(INCLUDEDIR)
          
VPATH = $(CMSISBASE)/DSP/Source/BasicMathFunctions/ \
        $(CMSISBASE)/DSP/Source/CommonTables/ \
        $(CMSISBASE)/DSP/Source/ComplexMathFunctions/ \
        $(CMSISBASE)/DSP/Source/ControllerFunctions/ \
        $(CMSISBASE)/DSP/Source/FastMathFunctions/ \
        $(CMSISBASE)/DSP/Source/FilteringFunctions/ \
        $(CMSISBASE)/DSP/Source/MatrixFunctions/ \
        $(CMSISBASE)/DSP/Source/StatisticsFunctions/ \
        $(CMSISBASE)/DSP/Source/SupportFunctions/ \
        $(CMSISBASE)/DSP/Source/TransformFunctions

.PHONY : clean all depend install uninstall

all: $(TARGET)

%.o.$(ARCH) : %.c
	$(COMPILE.c) $(OUTPUT_OPTION) $<

%.o.$(ARCH): %.s
	$(COMPILE.s) -o $@ $<

#%.o.$(ARCH): %.S
#	$(COMPILE.s) -o $@ $<

${TARGET}: ${OBJS}
	${AR} rc ${TARGET} ${OBJS}
	${RANLIB} ${TARGET}

install: $(TARGET)
	mkdir -p $(INSTALLLIB)
	cp $(TARGET) $(INSTALLLIB)

uninstall:
	rm $(INSTALLLIB)/$(TARGET)

clean:
	rm -f *.o.$(ARCH) ${TARGET}
