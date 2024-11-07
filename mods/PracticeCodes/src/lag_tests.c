#include <common.h>
#include <levelselect.h>
#include <custom_text.h>

enum Tests
{
    TEST_LOFTY_MUDADA,
    TEST_LOFTY_JUMP,
    TEST_TREE_TOPS_LIFECHEST,
    TEST_PASSAGE_BOOST,
    TEST_PASSAGE_LOOK_VORTEX,
    TEST_PASSAGE_ROCKET,
    TEST_COVE_END
};
int current_test_index = 0;


bool lag_test_mode = true;
int level_test_index = 0;

int frames_tested = 0;

bool is_finished_with_tests = false;

extern int inventory_timer;

TestsUpdate(void)
{
    if (_gameState == GAMESTATE_GAMEPLAY)
    {
        LoftyTests();
        TreeTopsTests();
        PassageTests();
        CoveTests();
        frames_tested++;

    }
    if (is_finished_with_tests)
    {
        DrawFinishedText();
    }

    NextTest();
}

//Tests
void LoftyTests()
{
    if (_levelID == LOFTY_CASTLE_ID)
    {
        if (current_test_index == TEST_LOFTY_MUDADA)
        {   // Near Mudada
            _spyro.position.x = 0x000150DD;
            _spyro.position.y = 0x00013738;
            _spyro.position.z = 0x00007166;
            _spyro.angle.yaw = 0x0920;

            _cameraPosition.x = 0x000158DB;
            _cameraPosition.y = 0x00013B83;
            _cameraPosition.z = 0x000073B3;

            _cameraAngle.yaw = 0x0941;
            _cameraAngle.pitch = 0x00A3;
        }
        if (current_test_index == TEST_LOFTY_JUMP)
        {   // Before Laggy Jump
            _spyro.position.x = 0x00016CFB;
            _spyro.position.y = 0x00013DDA;
            _spyro.position.z = 0x00006F66;
            _spyro.angle.yaw = 0x0890;

            _cameraPosition.x = 0x0001764C;
            _cameraPosition.y = 0x00013FF1;
            _cameraPosition.z = 0x000071E6;

            _cameraAngle.yaw = 0x0890;
            _cameraAngle.pitch = 0x0083;
        }
    }
}

void TreeTopsTests()
{
    if (_levelID == TREE_TOPS_ID)
    {
        if (current_test_index == TEST_TREE_TOPS_LIFECHEST)
        {   // First 120 Area
            _spyro.position.x = 0x0001863A;
            _spyro.position.y = 0x0000B51D;
            _spyro.position.z = 0x00009322;
            _spyro.angle.yaw = 0x0140;

            _cameraPosition.x = 0x00017DB3;
            _cameraPosition.y = 0x0000B081;
            _cameraPosition.z = 0x0000958F;

            _cameraAngle.yaw = 0x0143;
            _cameraAngle.pitch = 0x00A0;
        }
    }
}

void PassageTests()
{
    if (_levelID == DARK_PASSAGE_ID)
    {
        if (current_test_index == TEST_PASSAGE_BOOST)
        {   // Near Devil
            _spyro.position.x = 0x0001404C;
            _spyro.position.y = 0x0000C1F2;
            _spyro.position.z = 0x00009966;
            _spyro.angle.yaw = 0x0140;

            _cameraPosition.x = 0x000137C3;
            _cameraPosition.y = 0x0000BD55;
            _cameraPosition.z = 0x00009BD3;

            _cameraAngle.yaw = 0x0143;
            _cameraAngle.pitch = 0x00A0;
        }
        else if (current_test_index == TEST_PASSAGE_LOOK_VORTEX)
        {   // Looking at vortex
            _spyro.position.x = 0x00018455;
            _spyro.position.y = 0x000132AD;
            _spyro.position.z = 0x0000AB67;
            _spyro.angle.yaw = 0x0990;

            _cameraPosition.x = 0x00018BE2;
            _cameraPosition.y = 0x00013830;
            _cameraPosition.z = 0x0000ADF8;

            _cameraAngle.yaw = 0x099C;
            _cameraAngle.pitch = 0x0099;
        }
        else if (current_test_index == TEST_PASSAGE_ROCKET)
        {   // Rocket Area
            _spyro.position.x = 0x000160BC;
            _spyro.position.y = 0x0000B21C;
            _spyro.position.z = 0x0000B969;
            _spyro.angle.yaw = 0x0440;

            _cameraPosition.x = 0x000161B6;
            _cameraPosition.y = 0x0000A8BD;
            _cameraPosition.z = 0x0000BC86;

            _cameraAngle.yaw = 0x0444;
            _cameraAngle.pitch = 0x0081;
        }
    }
}

void CoveTests()
{
    if (_levelID == GNORC_COVE_ID)
    {
        if (current_test_index == TEST_COVE_END)
        {   // Near End
            _spyro.position.x = 0x0001E7D8;
            _spyro.position.y = 0x00018014;
            _spyro.position.z = 0x00005166;
            _spyro.angle.yaw = 0x0840;

            _cameraPosition.x = 0x0001F156;
            _cameraPosition.y = 0x00018103;
            _cameraPosition.z = 0x000053D1;

            _cameraAngle.yaw = 0x0840;
            _cameraAngle.pitch = 0x0086;
        }
    }
}


void NextTest()
{
    if (frames_tested == 380)
    {

        if (current_test_index == TEST_LOFTY_JUMP)
        {
            LevelSelectFunction(3, LEVEL_SELECT_R1);
        }
        else if (current_test_index == TEST_TREE_TOPS_LIFECHEST)
        {
            LevelSelectFunction(4, LEVEL_SELECT_L1);
        }
        else if (current_test_index == TEST_PASSAGE_ROCKET)
        {
            LevelSelectFunction(5, LEVEL_SELECT_L1);
        }
        else if (current_test_index == (TEST_COVE_END + 1))
        {
            is_finished_with_tests = true;
        }
    }

    // For level Select, wait a bit to reset variables, and increment current_test to the next test
    if (frames_tested > 390)
    {
        frames_tested = 0;
        current_test_index++;
        inventory_timer = 0;
    }
}

void DrawFinishedText()
{
    CapitalTextInfo finished_text_info = { 0 };
    finished_text_info.x = SCREEN_LEFT_EDGE + 100;
    finished_text_info.y = SCREEN_BOTTOM_EDGE - 10;
    finished_text_info.size = DEFAULT_SIZE;
    DrawTextCapitals("FINISHED WITH TESTS", &finished_text_info, DEFAULT_SPACING, MOBY_COLOR_PURPLE);
    RenderShadedMobyQueue();
}