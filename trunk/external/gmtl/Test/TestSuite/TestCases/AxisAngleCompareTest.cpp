
/************************************************************** ggt-head beg
 *
 * GGT: Generic Graphics Toolkit
 *
 * Original Authors:
 *   Allen Bierbaum
 *
 * -----------------------------------------------------------------
 * File:          $RCSfile: AxisAngleCompareTest.cpp,v $
 * Date modified: $Date: 2006/07/19 14:14:01 $
 * Version:       $Revision: 1.3 $
 * -----------------------------------------------------------------
 *
 *********************************************************** ggt-head end */
/*************************************************************** ggt-cpr beg
*
* GGT: The Generic Graphics Toolkit
* Copyright (C) 2001,2002 Allen Bierbaum
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
 ************************************************************ ggt-cpr end */
#include "AxisAngleCompareTest.h"
#include "../Suites.h"
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/MetricRegistry.h>

#include <gmtl/AxisAngle.h>
#include <gmtl/AxisAngleOps.h>

namespace gmtlTest
{
   CPPUNIT_TEST_SUITE_REGISTRATION(AxisAngleCompareTest);
   CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(AxisAngleCompareMetricTest, Suites::metric());

   template<typename T>
   class testEqual
   {
   public:
      static void go()
      {
         gmtl::AxisAngle<T> quat1, quat2;
         quat1.set( (T)1.0, (T)2.0, (T)34.0, (T)980 );
         quat1 = quat2;
         CPPUNIT_ASSERT( quat1 == quat2 );
         CPPUNIT_ASSERT( quat2 == quat1 );

         // Test that != works on all elements
         for (int j = 0; j < 4; ++j)
         {
            quat2[j] = (T)1221.0f;
            CPPUNIT_ASSERT(  (quat1 != quat2) );
            CPPUNIT_ASSERT( !(quat1 == quat2) );
            quat2[j] = quat1[j]; // put it back
         }

         // Test for epsilon equals working
         CPPUNIT_ASSERT( gmtl::isEqual( quat1, quat2 ) );
         CPPUNIT_ASSERT( gmtl::isEqual( quat1, quat2, (T)0.0f ) );
         CPPUNIT_ASSERT( gmtl::isEqual( quat2, quat1, (T)0.0f ) );
         CPPUNIT_ASSERT( gmtl::isEqual( quat2, quat1, (T)100000.0f ) );
         T eps = (T)10.0;
         for (int j = 0; j < 4; ++j)
         {
            quat2[j] = quat1[j] - (eps / (T)2.0);
            CPPUNIT_ASSERT(  gmtl::isEqual( quat1, quat2, eps ) );
            CPPUNIT_ASSERT( !gmtl::isEqual( quat1, quat2, (T)(eps / 3.0) ) );
            quat2[j] = quat1[j]; // put it back
         }
      }
   };

   void AxisAngleCompareTest::testAxisAngleEqualityFloatTest()
   {
      testEqual<float>::go();
      testEqual<float>::go();
      testEqual<float>::go();
      testEqual<float>::go();
      testEqual<float>::go();
      testEqual<float>::go();
      testEqual<float>::go();
      testEqual<float>::go();
      testEqual<float>::go();
      testEqual<float>::go();
      testEqual<float>::go();
      testEqual<float>::go();
      testEqual<float>::go();
      testEqual<float>::go();
   }

   void AxisAngleCompareTest::testAxisAngleEqualityDoubleTest()
   {
      testEqual<double>::go();
      testEqual<double>::go();
      testEqual<double>::go();
      testEqual<double>::go();
      testEqual<double>::go();
      testEqual<double>::go();
      testEqual<double>::go();
      testEqual<double>::go();
      testEqual<double>::go();
      testEqual<double>::go();
      testEqual<double>::go();
      testEqual<double>::go();
      testEqual<double>::go();
      testEqual<double>::go();
   }

   void AxisAngleCompareMetricTest::testAxisAngleTimingOpEqualityTest()
   {
      // Test overhead of creation
      const long iters(400000);

      gmtl::AxisAnglef src_quat11;
      gmtl::AxisAnglef src_quat22;
      gmtl::AxisAnglef src_quat33;
      gmtl::AxisAnglef src_quat34;
      gmtl::AxisAnglef src_quat44;
      gmtl::AxisAngled src_quat101;

      // half of them will be equal
      src_quat11[3] = 1.0f;
      src_quat22[2] = 1.0f;
      src_quat33[1] = 2.0f;

      gmtl::AxisAnglef test_quat11( src_quat11 );
      gmtl::AxisAnglef test_quat22( src_quat22 );
      gmtl::AxisAnglef test_quat33( src_quat33 );
      gmtl::AxisAnglef test_quat34( src_quat34 );
      gmtl::AxisAnglef test_quat44( src_quat44 );
      gmtl::AxisAngled test_quat101( src_quat101 );

      // half will be not equal
      src_quat34[0] = 2.0f;
      src_quat44[1] = 3.0f;
      src_quat101[3] = 1.0;

      unsigned true_count(0);

      CPPUNIT_METRIC_START_TIMING();
      for (long iter = 0; iter < iters; ++iter)
      {
         if (src_quat11 == test_quat11)
            ++true_count;
         if (src_quat22 == test_quat22)
            ++true_count;
         if (src_quat33 == test_quat33)
            ++true_count;
         if (src_quat34 == test_quat34)
            ++true_count;
         if (src_quat44 == test_quat44)
            ++true_count;
         if (src_quat101 == test_quat101)
            ++true_count;
      }

      CPPUNIT_METRIC_STOP_TIMING();
      CPPUNIT_ASSERT_METRIC_TIMING_LE("AxisAngleCompareTest/operator==", iters, 0.075f, 0.1f);  // warn at 7.5%, error at 10%

      // Make sure the compiler doesn't optimize out true_count
      CPPUNIT_ASSERT( true_count > 0 );
   }

   void AxisAngleCompareMetricTest::testAxisAngleTimingOpNotEqualityTest()
   {
      // Test overhead of creation
      const long iters(400000);

      gmtl::AxisAnglef src_quat11;
      gmtl::AxisAnglef src_quat22;
      gmtl::AxisAnglef src_quat33;
      gmtl::AxisAnglef src_quat34;
      gmtl::AxisAnglef src_quat44;
      gmtl::AxisAngled src_quat101;

      // half will be equal
      src_quat11[0] = 1.0f;
      src_quat22[2] = 1.0f;
      src_quat33[1] = 2.0f;

      gmtl::AxisAnglef test_quat11( src_quat11 );
      gmtl::AxisAnglef test_quat22( src_quat22 );
      gmtl::AxisAnglef test_quat33( src_quat33 );
      gmtl::AxisAnglef test_quat34( src_quat34 );
      gmtl::AxisAnglef test_quat44( src_quat44 );
      gmtl::AxisAngled test_quat101( src_quat101 );

      // half will be not equal
      src_quat34[0] = 2.0f;
      src_quat44[1] = 3.0f;
      src_quat101[3] = 1.0f;

      unsigned true_count(0);

      CPPUNIT_METRIC_START_TIMING();
      for( long iter=0;iter<iters; ++iter)
      {
         if (src_quat11 != test_quat11)
            ++true_count;
         if (src_quat22 != test_quat22)
            ++true_count;
         if (src_quat33 != test_quat33)
            ++true_count;
         if (src_quat34 != test_quat34)
            ++true_count;
         if (src_quat44 != test_quat44)
            ++true_count;
         if (src_quat101 != test_quat101)
            ++true_count;
      }

      CPPUNIT_METRIC_STOP_TIMING();
      CPPUNIT_ASSERT_METRIC_TIMING_LE("AxisAngleCompareTest/operator!=", iters, 0.075f, 0.1f);  // warn at 7.5%, error at 10%

      // Make sure the compiler doesn't optimize out true_count
      CPPUNIT_ASSERT( true_count > 0 );
   }

   void AxisAngleCompareMetricTest::testAxisAngleTimingIsEqualTest()
   {
      // Test overhead of creation
      const long iters(400000);

      gmtl::AxisAnglef src_quat11;
      gmtl::AxisAnglef src_quat22;
      gmtl::AxisAnglef src_quat33;
      gmtl::AxisAnglef src_quat34;
      gmtl::AxisAnglef src_quat44;
      gmtl::AxisAngled src_quat101;

      // half will be equal
      src_quat11[0] = 1.0f;
      src_quat22[1] = 1.0f;
      src_quat33[2] = 2.0f;

      gmtl::AxisAnglef test_quat11( src_quat11 );
      gmtl::AxisAnglef test_quat22( src_quat22 );
      gmtl::AxisAnglef test_quat33( src_quat33 );
      gmtl::AxisAnglef test_quat34( src_quat34 );
      gmtl::AxisAnglef test_quat44( src_quat44 );
      gmtl::AxisAngled test_quat101( src_quat101 );

      // half will be not equal
      src_quat34[0] = 23.0f;
      src_quat44[2] = 234.0f;
      src_quat101[3] = 1234.0f;

      unsigned true_count(0);

      CPPUNIT_METRIC_START_TIMING();
      for( long iter=0;iter<iters; ++iter)
      {
         if (gmtl::isEqual( src_quat11,  test_quat11, 0.0f ))
            ++true_count;
         if (gmtl::isEqual( src_quat22,  test_quat22, 0.2f ))
            ++true_count;
         if (gmtl::isEqual( src_quat33,  test_quat33, 0.3f ))
            ++true_count;
         if (gmtl::isEqual( src_quat34,  test_quat34, 0.6f ))
            ++true_count;
         if (gmtl::isEqual( src_quat44,  test_quat44, 0.8f ))
            ++true_count;
         /*
         if (gmtl::isEqual( src_quat101, test_quat101, 111.1f ))
            ++true_count;
            */
      }

      CPPUNIT_METRIC_STOP_TIMING();
      CPPUNIT_ASSERT_METRIC_TIMING_LE("AxisAngleCompareTest/isEqual(...)", iters, 0.075f, 0.1f);  // warn at 7.5%, error at 10%

      // Make sure the compiler doesn't optimize out true_count
      CPPUNIT_ASSERT( true_count > 0 );
   }
}