#include "include/levels.h"
#include <stdio.h>
#include <stdlib.h>

LevelData g_levels[LEVEL_COUNT];

void init_levels() {
    // 初始化默认数据
    for (int i = 0; i < LEVEL_COUNT; i++) {
        g_levels[i].id = (LevelID)i;
        g_levels[i].isUnlocked = false;
        g_levels[i].isCompleted = false;
        g_levels[i].restrictPieces = false;
        g_levels[i].garbageType = GARBAGE_RANDOM;
    }

    // --- PM-1 ---
    _stprintf(g_levels[LEVEL_1].name, _T("PM-1"));
    _stprintf(g_levels[LEVEL_1].title, _T("PM-1 天穹市"));
    _stprintf(g_levels[LEVEL_1].story, _T("引言：天穹市的起源？不不不，根本没有任何资料从起源那一代人那流传下来，本书只从最早被发现的资料开始记起。\n天穹市是一座被透明能量罩包裹起来的城市，只有接触到它的时候它才会显现出来。能量罩的核心能量由天穹塔提供，其他能量由在城市四个边界的支柱提供。\n天穹市坐落于一个巨大的盆地之中，对外沟通（应该有）是通过山下的隧道进行的。笔者从来没见过盆地外是怎样，地上的人们也没见过。\n天穹市的科技极其发达，据可考据史料，自天穹塔的超算单元更新与地下城建成之后，科技水平就快速发展，短短几十年，全息投影、脑机、意识上传技术都已经被攻克且已被投入使用，甚至有消息称短距离传送技术也正在研究中（消息截至日志208年）\n关于天穹塔：数百米高，是唯一能看到盆地外的地方。其1/3的部分是在能量罩之外，其余部分在能量罩之内。\n......\n关于外面：笔者曾实地考察过支柱地区，发现只有特定的军队能被允许出去，而且每次出去带回来的都是一些零件、日常资源。但是为什么需要军队？需要的不应该是商人吗？\n关于外面（补充）：笔者由于自身关系无法假装成军人，所以曾委托一个人假扮混出去，但是那个人再也没回来过，我也联系不到，难道是我被骗钱了？\n关于外面（后序）：花了大价钱买通了军队里的人，他告诉我外面都是沙子，有时候还会出现伏击的野兽，能量罩看到的东西都是虚假的。说话这句话后他的眼神变得很坚毅，像是要赴死，并让我将报酬转给他的家人。第二天就听说他死了。\n--- ■■■■《天穹日志》记于日志277年"));
    g_levels[LEVEL_1].targetLines = 10;
    g_levels[LEVEL_1].initialGarbageRows = 4;
    g_levels[LEVEL_1].garbageType = GARBAGE_RANDOM;
    g_levels[LEVEL_1].mapX = 100;
    g_levels[LEVEL_1].mapY = 400;
    g_levels[LEVEL_1].isUnlocked = true; // 第一关默认解锁

    // --- PM-2 ---
    _stprintf(g_levels[LEVEL_2].name, _T("PM-2"));
    _stprintf(g_levels[LEVEL_2].title, _T("PM-2 镜面"));
    _stprintf(g_levels[LEVEL_2].story, _T("日志325年。\n")
                                    _T("我站在天穹金融中心的顶端。此时，天穹市的市长正在中心广场上眉飞色舞地描绘着天穹市未来的发展方向，这让我感到厌恶，每次说着倾听群众的声音，却每次对意见单上排行第一的“外面”置之不理。\n")
                                    _T("“滴滴”，耳机里传来了一阵电话铃声。\n“这时候打来，万一我已经在行动了，不就暴露了吗。”我嘀咕道。但由于是委托人的电话，所以我不得不接起。\n“别忘记了，干掉那个市长，不限形式，但最好动静弄大一点，这样我们这边比较方便行动。”依旧是经过处理的中性声。实话实说，我跟这位奇怪的委托人连面都没有见过，更别说知道他们要干什么了。\n“我会安排的。”我敷衍了一下，随即挂断了电话。大动静的刺杀，这并不符合我的行事风格。不过我可以先杀了他，然后再通过骇入周边大厦的全息屏，然后播放精心拍摄的击杀回放，这应该也算大动静吧？我给我自己的定位就是打响第一枪，至于后面如何，那可不关我的事。\n")
                                    _T("我深吸一口气，戴上帽子。天穹金融中心的顶端距离目标大厦只有十七米的水平距离，五米的高度差，这对我特制的鞋子来说是个轻松的起跳。全息投影的霓虹色带在我身旁流转，足以掩盖我的身影。就在我掠过两座大厦之间虚空的瞬间，一道近乎透明的能量束擦过我的左肩。不等护目镜上出现红色的警报，我已经本能地在空中翻滚，强行扭身抓住了目标大厦边缘的检修梯。金属梯子在冲击下发出痛苦的呻吟。\n")
                                    _T("“就知道没这么简单。”我咬牙攀上屋顶，小臂上的装甲瞬间变形，化为约一米长的利刃握在手中。\n")
                                    _T("伏击者穿着与城市夜色融为一体的光学迷彩，只有在他移动时才能在空气中看到细微的扭曲。我只好缓缓地移动，并集中精神注意周围的变化以捕捉他的身影。突然，一阵划破空气的声音响起。他出手了。我下意识地抬起手抵挡。剑刃与我的利刃碰撞出刺眼的蓝白色火花，震得我整条手臂发麻。这一刻，我看清了他的身影。衣着不符合现代样式，很像史料中记载的“原住民”。但是为什么高塔上会有“原住民”？外骨骼的动能吸收模块疯狂运转，将冲击力转化为储能，我能感受到能量在背部导管中奔流的灼痛。他几次突刺都快得只留下残影，我只能凭借外骨骼的预判算法勉强格挡。每次剑刃相交，空气中都会爆发出臭氧的味道。\n")
                                    _T("“自由刺客？”他在第三次交击时突然开口，声音经过变声器处理，像金属摩擦，“还是雇佣兵？”\n我没有回答，而是故意卖出一个破绽，左肋露出空档。他果然上钩，光剑以一个刁钻的角度刺来。但我的外骨骼早就计算好了轨迹——我向后折叠身体，脊柱几乎要断成两截，利刃沿着他的剑刃滑向剑柄，然后猛地向上挑起。\n")
                                    _T("等离子刀刃深深切入了他的胸口。他倒下的瞬间，身体抽搐着，光学迷彩失效，露出了衣服下的机械身体。我虽然并不意外遭到安保伏击的清空，但还是对一个类似“原住民”的机械人感到疑惑。他的样子，并不像伏击，而是像侵犯了他的领地。\n正当我调整思绪，准备寻找狙击点时，突然感觉头很晕。\n“刚才被射到的那一剑原来有毒吗？“我伸手想去触摸伤口，可是眼睛一黑，我倒在了地上。\n……\n")
                                    _T("画面重新亮起时，我感到了不对劲。我发现我正靠坐在一个角落，握着一把尚有余温的光剑。由于我的头晕还没从之前缓解过来，所以我暂时先闭目养神。不知过了多久，突然，一阵强烈的电信号惊醒了我，我向着我脑中指向的方向看去。面前不远处，一个熟悉的身影正从金融中心的天台起跳——那是我自己。\n我的手不受控制地将身旁的其中一把光剑拿起，然后向目标射去，和刚才的发展一样，长得跟我一样的人本能地在空中翻滚，强行扭身抓住了目标大厦边缘的检修梯。金属梯子在冲击下发出痛苦的呻吟。不过有一处不一样，就是我愣在原地，对眼前一切都还没适应，等到我意识到我的迷彩还没开的瞬间，我的胸口已经被利刃贯穿，然后视线又黑了过去。\n......\n")
                                    _T("画面重新亮起时，我正靠坐在一个角落，握着一把尚有余温的光剑。\n“看来距离那个我调过来，我还有有时间调查。”我意念一动，全身便与周围融为一体。我稍微看了看周围的物件，也就只有一本日记本，捡起打开一看，也就只有“镜面”，“自尽”两个词语。不等我思考有什么联系，突然，一阵强烈的电信号惊醒了我，我向着我脑中指向的方向看去。面前不远处，一个熟悉的身影正从金融中心的天台起跳——那是我自己。我大惊，我连忙想按住我的手，但是扔出光剑犹如思想钢印一般，我必须执行这段命令。在这慌慌张张之下，我又被结束了生命。\n......\n")
                                    _T("我决定做个实验。来来回回又死了几次之后，我发现只有发现敌人、扔出光剑是不可控的，其他都由自己决定，然后我有思考起了那几个词“镜面”，“自尽”。\n“自尽吗？”我恍然大悟，拿出光剑，对准自己的胸口刺去，看着自己的身体渐渐倒下，我觉得我应该触碰到了真相。\n......\n再次醒来时，我已将光剑投出，并且此时剑刃与”我“的利刃碰撞出刺眼的蓝白色火花。我感到非常慌张，竭力想收剑然后尝试解释。只可惜，身体变得不可控了。\n混乱还没来得及在脑中形成完整的思绪，一道命令如烧红的烙铁般直接烙进了我的意识皮层：\n“清除跨越不可知的奇点”\n这不是声音，不是文字，是某种更高优先级的指令，像心跳一样不可违抗。我的新身体——这个被我杀死的伏击者的身体——开始不受控制地转动光剑。我惊恐地发现，此时我的记忆被这副身体疯狂地吸去。\n")
                                    _T("对面的“我”显然也察觉到了异常，在半空中强行调整了落点。但已经晚了。这个身体的肌肉记忆比我的意识更快，并且像是完全了解我的动作习惯、思绪一样。光剑以一个刁钻的角度刺出，精准地贯穿了我的心脏。\n那个穿着黑色风衣的我——像折翼的鸟一样坠落。在落地前的最后一秒，我看见“我”的眼睛里充满了困惑和不甘。\n世界再次陷入黑暗。\n……\n剧痛。\n然后是刺眼的光芒。我发现自己站在广场的演讲台上，随后，我的身躯正笨拙地向后倒去。我的胸口被某种高能武器贯穿，温热的液体正迅速带走体温。耳边还回荡着市民们惊恐的尖叫。我感觉我身体的每个部分都变得无法动弹，先是四肢，然后是思考能力，最后是躯体。\n")
                                    _T("“现在变成了市长吗？不过，也算是任务成功了”我苦笑道，不知道还会不会有再一次身份的反转，只是静静地等待死去。\n"));
    g_levels[LEVEL_2].targetLines = 17;
    g_levels[LEVEL_2].initialGarbageRows = 5;
    g_levels[LEVEL_2].garbageType = GARBAGE_RANDOM;
    g_levels[LEVEL_2].mapX = 300;
    g_levels[LEVEL_2].mapY = 300;

    // --- PM-3 ---
    _stprintf(g_levels[LEVEL_3].name, _T("PM-3"));
    _stprintf(g_levels[LEVEL_3].title, _T("PM-3 Raven's"));
    _stprintf(g_levels[LEVEL_3].story, _T("Raven's建在地下城的一处不起眼的角落，处理着天穹市的大部分黑白情报。\n--- ■■■■《天穹日志》记于日志307年\n")
                                    _T("布朗尼，通称“骇兔”，一名活跃于赛博都市“天穹城”中的天才骇客。\n她喜欢有风险的挑战，渴望刺激的冒险，并将其视为自己人生的最大乐趣。\n虽然她也曾多次身处险境，但只要还有枪和无人机在她的身旁，那她就永远都不会失败。\n是的，“骇兔”的计划，永远都不会失败。\n--- ■■■■《天穹日志》\n")
                                    _T("少女又看了看口袋里的计时器。\n距离约定的交易时间还有4分53秒，但她预期中的脚步声却并未出现。\n向她靠近的，她所能听到的，就只有某种潮湿之物贴地蠕动所发出的粘稠的声响。\n她并不喜欢这样的声响。可它却又不受控制地，蹭着她的肌肤滑进她的耳朵里。简直就像是——\n蛇。\n？？？：您点的「生肉」。\n")
                                    _T("灯光亮起，视觉代替听觉优先做出了判断。黑衣的男人撑着一把黑伞，提着颇为沉重的箱子。\n？？？：在下，灰蛇。\n布朗尼：呵，来的倒是还挺准时的。\n灰蛇：并非准时。相比于约定的时间，在下提前了4分15秒抵达。\n布朗尼：哈，有趣！这就是你们「蛇眼」的职业水准吗？倒是的确比那些街头贩子的水平高了不少啊！\n")
                                    _T("灰蛇：布朗尼小姐过奖了。提供最能符合顾客口味的「生肉」，这是「蛇眼」对自身的要求，而非追求。\n灰蛇的声音呈现出了某种不自然的停顿。\n灰蛇：希望今天的这份「生肉」，足以回应布朗尼小姐您在此共计37分08秒的等待。\n布朗尼：哼，监视顾客的一举一动难道也是你们「蛇眼」的职业水准吗？\n灰蛇：在过去的半小时里，您不也一直在监视着我的动向么，布朗尼小姐？\n")
                                    _T("灰蛇：只不过，在您关注「蛇眼」…关注我的这段时间里，「蛇」早已完成了它的使命。\n布朗尼：哇，那你真的是好棒棒呢~要我给你鼓个掌吗？\n灰蛇：呵呵……\n男人发出了颇为诡异的笑声，或者说更像是蛇吐信子的嘶嘶声。\n少女颇为不快地瞥了男人一眼，随后接过了他手中的箱子。\n灰蛇：请在四维圆电子板上签字，布朗尼小姐。对，就是现在显示的位置。\n")
                                    _T("布朗尼摘下手套，正准备签下自己的专属源码。而就在这时，那些跳动的字形突然剧烈地闪动了一下，消失了。\n布朗尼：根据附录一，我可以选择现在「解冻」。\n灰蛇：当然。一切遵从您的意愿，布朗尼小姐。\n灰蛇：但在下必须提醒您，根据附录二第三条，「解冻」期间内的一切风险都将由您，尊敬的布朗尼小姐，一人承担。\n")
                                    _T("布朗尼：我知道。第三条到第五条就是我要求加上去的。\n布朗尼：虽然你们「蛇眼」办事一直很稳，但不亲自验一下这「肉」的质量，总还是放不下心。\n灰蛇：您很谨慎，这对我们都好。\n布朗尼将手放在了箱子中间的圆盘上。随后，一阵机械的咔咔声响起，像是一道道精密的锁扣逐个解开。\n万物静寂，似乎是在等待着箱子应声弹开时，那一声令人安心的「咔嚓」声。\n")
                                    _T("但什么都没有发生。只有少女的指尖——确切地说，是少女指尖前几毫米的虚空，被点亮了。\n无声的信息流在她的指尖跳动着。在她的视野里，无数繁杂的噪点迅速组合成了诸多可视的图像，而后又即刻消散。\n布朗尼：可视化交互演示地图…这是他们给自己的客户准备的……\n布朗尼：驻点执勤检查，监控终端的分布和辅助定位点……\n")
                                    _T("布朗尼：最近三个月的告警记录和优化报告，报警阈值和等级的分析……\n无边无际的信息流终于照亮了兜帽暗处，少女那不自觉向上弯起的嘴角。\n她熟练地在圆盘上划出若干个手势。投影中的球体也随之在不同的节点上记录了数十条加密输入的信息——签名完成。\n布朗尼：确实值得我承担这个风险…你们提供的的确是我最想要的那块好「肉」。\n")
                                    _T("布朗尼：嗯…是桩好买卖。\n灰蛇：嗯。的确，是桩好买卖。\n黑衣的男人有意无意地重复着她的话，声音里似乎带着一丝笑意。\n灰蛇：不过。旋那边的任务似乎完成了。不准备去迎接一下吗。\n布朗尼：看在今天这「肉」的份上……\n灰蛇：后会有期了，小兔子。Bless.\n")
                                    _T("城市中的广播仍在循环播放那讨厌的宣传语：\n-在天穹市，每个人都有独一无二的价值。\n-无所不能的智能科技，无处不在的信息网络……天穹塔，将为您的自由与安全提供最优质的保障。\n-无忧无虑的全新生活，只在——\n-「光幕」下的天穹。\n“早晚得把这地方给拆了。”\n"));
    g_levels[LEVEL_3].targetLines = 24;
    g_levels[LEVEL_3].initialGarbageRows = 6;
    g_levels[LEVEL_3].garbageType = GARBAGE_RANDOM;
    g_levels[LEVEL_3].mapX = 500;
    g_levels[LEVEL_3].mapY = 400;

    // --- PM-4 ---
    _stprintf(g_levels[LEVEL_4].name, _T("PM-4"));
    _stprintf(g_levels[LEVEL_4].title, _T("PM-4 星光"));
    _stprintf(g_levels[LEVEL_4].story, _T("当你再次醒来的时候，你躺在一个名叫Raven's咖啡馆里。面前两个人告诉你她们一个是这里的店长渡鸦，一个是前台布朗尼。"));
    g_levels[LEVEL_4].targetLines = 31;
    g_levels[LEVEL_4].initialGarbageRows = 7;
    g_levels[LEVEL_4].garbageType = GARBAGE_RANDOM;
    g_levels[LEVEL_4].mapX = 700;
    g_levels[LEVEL_4].mapY = 300;

    // --- PM-5 ---
    _stprintf(g_levels[LEVEL_5].name, _T("PM-5"));
    _stprintf(g_levels[LEVEL_5].title, _T("PM-5 注目礼"));
    _stprintf(g_levels[LEVEL_5].story, _T("在作战中心，你见到了此次作战的负责人德尔塔，她告诉你她负责对付防卫军的最高领袖霞。"));
    g_levels[LEVEL_5].targetLines = 38;
    g_levels[LEVEL_5].initialGarbageRows = 8;
    g_levels[LEVEL_5].garbageType = GARBAGE_RANDOM;
    g_levels[LEVEL_5].mapX = 900;
    g_levels[LEVEL_5].mapY = 400;

    // --- PM-6 ---
    _stprintf(g_levels[LEVEL_6].name, _T("PM-6"));
    _stprintf(g_levels[LEVEL_6].title, _T("PM-6 绘于天空之梦"));
    _stprintf(g_levels[LEVEL_6].story, _T("天空为人们呈现的一切，都是虚假的，就连白天昼夜，都是通过能量罩模拟的。"));
    g_levels[LEVEL_6].targetLines = 45;
    g_levels[LEVEL_6].initialGarbageRows = 9;
    g_levels[LEVEL_6].garbageType = GARBAGE_RANDOM;
    g_levels[LEVEL_6].mapX = 1100;
    g_levels[LEVEL_6].mapY = 300;

    // --- PM-7 ---
    _stprintf(g_levels[LEVEL_7].name, _T("PM-7"));
    _stprintf(g_levels[LEVEL_7].title, _T("PM-7 拂晓之前"));
    _stprintf(g_levels[LEVEL_7].story, _T("数小时后，拂晓之前，天穹城。"));
    g_levels[LEVEL_7].targetLines = 52;
    g_levels[LEVEL_7].initialGarbageRows = 10;
    g_levels[LEVEL_7].garbageType = GARBAGE_RANDOM;
    g_levels[LEVEL_7].mapX = 1300;
    g_levels[LEVEL_7].mapY = 400;

    // --- PM-8 ---
    _stprintf(g_levels[LEVEL_8].name, _T("PM-8"));
    _stprintf(g_levels[LEVEL_8].title, _T("PM-8 ■ ■ ■ ■"));
    _stprintf(g_levels[LEVEL_8].story, _T("在天穹塔的超算核心，你见到了《天穹日志》的作者。你们很奇怪为什么一个天穹塔的核心，却记录着天穹市发生的所有事情，无论是对天穹市有利的还不不利的。"));
    g_levels[LEVEL_8].targetLines = 59;
    g_levels[LEVEL_8].initialGarbageRows = 11;
    g_levels[LEVEL_8].garbageType = GARBAGE_RANDOM;
    g_levels[LEVEL_8].mapX = 1500;
    g_levels[LEVEL_8].mapY = 300;

    // --- PM-9 ---
    _stprintf(g_levels[LEVEL_9].name, _T("PM-9"));
    _stprintf(g_levels[LEVEL_9].title, _T("PM-9 妥协"));
    _stprintf(g_levels[LEVEL_9].story, _T("404NOTFOUND"));
    g_levels[LEVEL_9].targetLines = 66;
    g_levels[LEVEL_9].initialGarbageRows = 12;
    g_levels[LEVEL_9].garbageType = GARBAGE_RANDOM;
    g_levels[LEVEL_9].mapX = 1700;
    g_levels[LEVEL_9].mapY = 400;

    // --- PM-10 ---
    _stprintf(g_levels[LEVEL_10].name, _T("PM-10"));
    _stprintf(g_levels[LEVEL_10].title, _T("PM-10 天空井"));
    _stprintf(g_levels[LEVEL_10].story, _T("人们小时候就在想，只有天穹塔能高于虚假之天，那上面的人们，能看到什么？\n---- 隐德来希《天穹日志》\n从天穹塔向下望的那一刻才明白。所谓天穹市，不过就是一个巨大的井。\n人们以年为单位策划的行动，现在在你看来，不过是奔向另一场灭亡。"));
    g_levels[LEVEL_10].targetLines = 70;
    g_levels[LEVEL_10].initialGarbageRows = 14;
    g_levels[LEVEL_10].garbageType = GARBAGE_RANDOM;
    g_levels[LEVEL_10].mapX = 1900;
    g_levels[LEVEL_10].mapY = 300;

    // --- PM-EX-1 ---
    _stprintf(g_levels[LEVEL_EX_1].name, _T("PM-EX-1"));
    _stprintf(g_levels[LEVEL_EX_1].title, _T("PM-EX-1 因为山就在那儿"));
    _stprintf(g_levels[LEVEL_EX_1].story, _T("「山在那儿，总会有人去登的。」\n是，登山是智慧生物的一个本性，他们都想站得更高些，看得更远些，这并不是生存的需要。比如你，如果为了生存就会远远逃离这山，可你却登上来了。进化赋予智慧文明登高的欲望是有更深的原因的，这原因是什么我们还不知道。山无处不在，我们都还在山脚下。"));
    g_levels[LEVEL_EX_1].targetLines = 40;
    g_levels[LEVEL_EX_1].initialGarbageRows = 14;
    g_levels[LEVEL_EX_1].garbageType = GARBAGE_CONVEX;
    g_levels[LEVEL_EX_1].mapX = 2100;
    g_levels[LEVEL_EX_1].mapY = 200;

    // --- PM-EX-2 ---
    _stprintf(g_levels[LEVEL_EX_2].name, _T("PM-EX-2"));
    _stprintf(g_levels[LEVEL_EX_2].title, _T("PM-EX-2 坐天观井"));
    _stprintf(g_levels[LEVEL_EX_2].story, _T("xxx"));
    g_levels[LEVEL_EX_2].targetLines = 50;
    g_levels[LEVEL_EX_2].initialGarbageRows = 14;
    g_levels[LEVEL_EX_2].garbageType = GARBAGE_CONCAVE;
    g_levels[LEVEL_EX_2].mapX = 2300;
    g_levels[LEVEL_EX_2].mapY = 500;

    // --- PM-EX-3 ---
    _stprintf(g_levels[LEVEL_EX_3].name, _T("PM-EX-3"));
    _stprintf(g_levels[LEVEL_EX_3].title, _T("PM-EX-3 下一站"));
    _stprintf(g_levels[LEVEL_EX_3].story, _T("你已经神志不清了。你感觉天的彼端正延伸出一阶阶台阶向你而来。这回是你的下一站吗。"));
    g_levels[LEVEL_EX_3].targetLines = 30;
    g_levels[LEVEL_EX_3].initialGarbageRows = 0; // 无初始垃圾
    g_levels[LEVEL_EX_3].restrictPieces = true;
    g_levels[LEVEL_EX_3].mapX = 2500;
    g_levels[LEVEL_EX_3].mapY = 350;

    // --- PM-OS-1 ---
    _stprintf(g_levels[LEVEL_OS_1].name, _T("PM-OS-1"));
    _stprintf(g_levels[LEVEL_OS_1].title, _T("PM-OS-1 行于流逝的岸"));
    _stprintf(g_levels[LEVEL_OS_1].story, _T("")); // 不显示在剧情模式
    g_levels[LEVEL_OS_1].targetLines = 15;
    g_levels[LEVEL_OS_1].initialGarbageRows = 4;
    g_levels[LEVEL_OS_1].garbageType = GARBAGE_RANDOM;
    g_levels[LEVEL_OS_1].periodicReset = true;
    g_levels[LEVEL_OS_1].mapX = 2700;
    g_levels[LEVEL_OS_1].mapY = 200;

    // --- PM-OS-2 ---
    _stprintf(g_levels[LEVEL_OS_2].name, _T("PM-OS-2"));
    _stprintf(g_levels[LEVEL_OS_2].title, _T("PM-OS-2 游戏尘寰"));
    _stprintf(g_levels[LEVEL_OS_2].story, _T(""));
    g_levels[LEVEL_OS_2].targetLines = 15;
    g_levels[LEVEL_OS_2].initialGarbageRows = 0;
    g_levels[LEVEL_OS_2].garbageType = GARBAGE_NONE;
    g_levels[LEVEL_OS_2].unstablePieces = true;
    g_levels[LEVEL_OS_2].mapX = 2900;
    g_levels[LEVEL_OS_2].mapY = 300;
}

void generate_level_garbage(PlayerState *p, LevelID id) {
    LevelData *level = &g_levels[id];
    int rows = level->initialGarbageRows;
    if (rows <= 0) return;

    // 确保不超出棋盘
    if (rows >= BOARD_ROWS) rows = BOARD_ROWS - 1;

    // 从底部开始填充
    int startRow = BOARD_ROWS - rows;

    for (int r = startRow; r < BOARD_ROWS; r++) {
        // 根据类型生成行
        if (level->garbageType == GARBAGE_RANDOM) {
            // 随机生成一行，留1-2个空位
            for (int c = 0; c < BOARD_COLS; c++) {
                if (rand() % 10 < 8) { // 80% 概率有方块
                     p->board[r][c] = 1; // 简单的灰色块
                     p->boardColors[r][c] = RGB(100, 100, 100);
                } else {
                     p->board[r][c] = 0;
                     p->boardColors[r][c] = BLACK;
                }
            }
            // 确保至少有一个空位，防止一开始就消除了
            int hole = rand() % BOARD_COLS;
            p->board[r][hole] = 0;
            p->boardColors[r][hole] = BLACK;

        } else if (level->garbageType == GARBAGE_CONVEX) {
            // 凸型：中间高，两边低
            // 计算当前行相对于底部的距离 (0是底部)
            int distFromBottom = BOARD_ROWS - 1 - r;
            // 越高越窄
            // 底部宽度 10, 每向上几行缩减宽度
            int width = 10 - distFromBottom * 2; 
            if (width < 2) width = 2;
            
            int startCol = (BOARD_COLS - width) / 2;
            int endCol = startCol + width;
            
            for (int c = 0; c < BOARD_COLS; c++) {
                if (c >= startCol && c < endCol) {
                    p->board[r][c] = 1;
                    p->boardColors[r][c] = RGB(100, 100, 100);
                } else {
                    p->board[r][c] = 0;
                    p->boardColors[r][c] = BLACK;
                }
            }

        } else if (level->garbageType == GARBAGE_CONCAVE) {
            // 凹型：两边高，中间低
            int distFromBottom = BOARD_ROWS - 1 - r;
            // 越高中间空隙越大
            int gap = distFromBottom * 2;
            if (gap > 8) gap = 8;
            
            int startGap = (BOARD_COLS - gap) / 2;
            int endGap = startGap + gap;
            
            for (int c = 0; c < BOARD_COLS; c++) {
                if (c < startGap || c >= endGap) {
                    p->board[r][c] = 1;
                    p->boardColors[r][c] = RGB(100, 100, 100);
                } else {
                    p->board[r][c] = 0;
                    p->boardColors[r][c] = BLACK;
                }
            }
        }
    }
}

LevelData* get_level_data(LevelID id) {
    if (id < 0 || id >= LEVEL_COUNT) return NULL;
    return &g_levels[id];
}

bool is_level_completed(LevelID id) {
    if (id < 0 || id >= LEVEL_COUNT) return false;
    return g_levels[id].isCompleted;
}

void set_level_completed(LevelID id, bool completed) {
    if (id < 0 || id >= LEVEL_COUNT) return;
    g_levels[id].isCompleted = completed;
}

void unlock_next_level(LevelID currentId) {
    if (currentId + 1 < LEVEL_COUNT) {
        g_levels[currentId + 1].isUnlocked = true;
        // 更新最大解锁关卡索引（用于存档）
        if ((int)currentId + 1 > g_settings.maxUnlockedLevel) {
            g_settings.maxUnlockedLevel = (int)currentId + 1;
        }
        // 同时更新剧情解锁进度
        if ((int)currentId + 1 > g_settings.maxUnlockedStoryLevel) {
            g_settings.maxUnlockedStoryLevel = (int)currentId + 1;
        }
        save_settings();
    }
}
