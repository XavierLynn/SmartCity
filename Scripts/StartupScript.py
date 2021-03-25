

from pyecharts.charts import Bar
from pyecharts import options as opts


# def sayHello():
#     print("Hello")
bar = Bar()
#指定柱状图的横坐标
bar.add_xaxis(["衬衫", "毛衣", "领带", "裤子", "风衣", "高跟鞋", "袜子"])
#指定柱状图的纵坐标，而且可以指定多个纵坐标
bar.add_yaxis("商家A", [114, 55, 27, 101, 125, 27, 105])
bar.add_yaxis("商家B", [57, 55, 27, 101, 125, 27, 105])
#指定柱状图的标题
bar.set_global_opts(title_opts=opts.TitleOpts(title="某商场销售情况"))
#参数指定生成的html名称
bar.render('price.html')