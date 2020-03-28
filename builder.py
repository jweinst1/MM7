from __future__ import print_function
import sys
import random
import shutil
import subprocess

CLEANUP_FLAG = '--cleanup'
CLEANUP_BOOL = False

TEST_FLAG = '--test'
TEST_BOOL = False

ON_WINDOWS = sys.platform == 'win32'

TEMP_DIR_NAME = 'temp_' + str(random.getrandbits(50))

if __name__ == '__main__':
    user_args = sys.argv[1:]
    
    for arg in user_args:
        if arg == CLEANUP_FLAG:
            CLEANUP_BOOL = True
        if arg == TEST_FLAG:
            TEST_BOOL = True
            
    try:
        os.mkdir(TEMP_DIR_NAME)
    except OSError as exc:
        print("Got OS error while creating build dir, err=" + str(exc))
        sys.exit(3)
    
    os.chdir(TEMP_DIR_NAME)
    
    try:
        cmake_out = subprocess.check_output(['cmake', '..'])
        print(cmake_out)
    except subprocess.CalledProcessError as exc:
        print("Encountered error while running cmake")
        print("Got exitcode=" + str(exc.returncode))
        print("Got output=" + str(exc.output))
        sys.exit(3)
    
    
    if ON_WINDOWS:
        build_cmd = ["msbuild", "ALL_BUILD.vcxproj"]
    else:
        build_cmd = ["make"]
    
    try:
        build_output = subprocess.check_output(build_cmd)
        print(build_output)
    except subprocess.CalledProcessError as exc:
        print("Encountered a compile time error")
        print("output=" + str(exc.output))
        sys.exit(4)
        
    if TEST_BOOL:
        if ON_WINDOWS:
            try:
                test_output = subprocess.check_output(["msbuild", "RUN_TESTS.vcxproj"])
                print(test_output)
            except subprocess.CalledProcessError as exc:
                print("Encountered error while running tests")
                print("testing err=" + str(exc.output))
        else:
            try:
                test_output = subprocess.check_output(["make", "test"])
                print(test_output)
            except subprocess.CalledProcessError as exc:
                print("Encountered error while running tests")
                print("testing err=" + str(exc.output))         
        
    
    if CLEANUP_BOOL:
        os.chdir('..')
        shutil.rmtree(TEMP_DIR_NAME)
        